// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_aligned_storage.h"
#include "daw/daw_compiler_fixups.h"
#include "daw/daw_exception.h"
#include "daw/daw_move.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"
#include "daw/daw_virtual_base.h"
#include "daw/impl/daw_make_trait.h"
#include "daw/traits/daw_traits_remove_cvref.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <new>
#include <type_traits>

#if defined( DAW_HAS_GCC )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

namespace daw {
	namespace func_impl {
		DAW_MAKE_REQ_TRAIT( is_boolable_v,
		                    static_cast<bool>( std::declval<T>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_empty_member_v, std::declval<T>( ).empty( ) );

		template<size_t StorageSize, typename Base>
		struct function_storage {
			daw::aligned_storage_t<StorageSize> m_data;

			~function_storage( ) {
				clean( );
			}

			function_storage( function_storage const & ) = default;
			function_storage( function_storage && ) = default;

			function_storage &operator=( function_storage const &rhs ) {
				if( this == &rhs ) {
					return *this;
				}
				clean( );
				m_data = rhs.m_data;
				return *this;
			}

			function_storage &operator=( function_storage &&rhs ) noexcept {
				if( this == &rhs ) {
					return *this;
				}
				clean( );
				m_data = std::move( rhs.m_data );
				return *this;
			}

			template<
			  typename Func,
			  std::enable_if_t<!std::is_same_v<std::decay_t<Func>, function_storage>,
			                   std::nullptr_t> = nullptr>
			function_storage( Func &&f ) {
				static_assert( sizeof( std::decay_t<Func> ) <= StorageSize );
				store( DAW_FWD( f ) );
			}

			[[nodiscard]] Base *ptr( ) noexcept {
				return reinterpret_cast<Base *>( &m_data );
			}

			[[nodiscard]] Base const *ptr( ) const noexcept {
				return reinterpret_cast<Base const *>( &m_data );
			}

			void clean( ) {
				std::destroy_at( ptr( ) );
			}

			template<typename Func>
			void store( Func &&f ) {
				static_assert( sizeof( std::decay_t<Func> ) <= StorageSize );
				static_assert( std::is_base_of_v<Base, Func>,
				               "Can only store children of func_base" );
				new( &m_data ) daw::remove_cvref_t<std::decay_t<Func>>( DAW_FWD( f ) );
			}

			[[nodiscard]] Base &operator*( ) noexcept {
				return *ptr( );
			}

			[[nodiscard]] Base const &operator*( ) const noexcept {
				return *ptr( );
			}

			[[nodiscard]] Base *operator->( ) noexcept {
				return ptr( );
			}

			[[nodiscard]] Base const *operator->( ) const noexcept {
				return ptr( );
			}
		};

		template<typename Result, typename... FuncArgs>
		struct function_base
		  : daw::virtual_base<function_base<Result, FuncArgs...>> {
			function_base( ) = default;

			virtual Result operator( )( FuncArgs... ) const = 0;
			virtual Result operator( )( FuncArgs... ) = 0;
			[[nodiscard]] virtual bool empty( ) const = 0;
		};

		template<typename Result, typename... FuncArgs>
		struct empty_child final : function_base<Result, FuncArgs...> {
			[[noreturn]] Result operator( )( FuncArgs... ) const override {
				daw::exception::daw_throw<std::bad_function_call>( );
			}

			[[noreturn]] Result operator( )( FuncArgs... ) override {
				daw::exception::daw_throw<std::bad_function_call>( );
			}

			[[nodiscard]] bool empty( ) const override {
				return true;
			}
		};

		template<typename...>
		struct function_child;

		template<typename Func, typename Result, typename... FuncArgs>
		struct function_child<Func, Result, FuncArgs...> final
		  : function_base<Result, FuncArgs...>,
		    enable_copy_constructor<Func>,
		    enable_move_constructor<Func>,
		    enable_copy_assignment<Func>,
		    enable_move_assignment<Func> {
			Func m_func;

			template<typename F, std::enable_if_t<
			                       !std::is_same_v<std::decay_t<F>, function_child>,
			                       std::nullptr_t> = nullptr>
			function_child( F &&func ) noexcept(
			  std::is_nothrow_constructible_v<Func, F> )
			  : m_func( DAW_FWD( func ) ) {}

			Result operator( )( FuncArgs... args ) override {
				daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
				if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
					// daw::invoke( m_func, std::move( args )... );
					m_func( args... );
					return;
				} else {
					// return daw::invoke( m_func, std::move( args )... );
					return m_func( args... );
				}
			}

			Result operator( )( FuncArgs... args ) const override {
				daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
				if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
					// daw::invoke( m_func, std::move( args )... );
					m_func( args... );
					return;
				} else {
					// return daw::invoke( m_func, std::move( args )... );
					return m_func( args... );
				}
			}

		private:
			template<typename F>
			[[nodiscard]] static constexpr bool empty_impl( F &&f ) {
				if constexpr( func_impl::has_empty_member_v<F> ) {
					return f.empty( );
				} else if constexpr( func_impl::is_boolable_v<F> ) {
					return not static_cast<bool>( f );
				} else {
					return false;
				}
			}

		public:
			[[nodiscard]] bool empty( ) const override {
				return empty_impl( m_func );
			}
		};

		template<size_t MaxSize, typename function_base, typename function_child,
		         typename empty_child, typename Func>
		function_storage<MaxSize, function_base> store_if_not_empty( Func &&f ) {
			static_assert( sizeof( std::decay_t<Func> ) <= MaxSize );
			if constexpr( func_impl::has_empty_member_v<Func> ) {
				if( f.empty( ) ) {
					return { empty_child( ) };
				}
			} else if constexpr( func_impl::is_boolable_v<Func> ) {
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#pragma GCC diagnostic ignored "-Waddress"
#endif
				if constexpr( not std::is_function_v<daw::remove_cvref_t<Func>> ) {
					if( not f ) {
						return { empty_child( ) };
					}
				}
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic pop
#endif
			}
			return { function_child( DAW_FWD( f ) ) };
		}

		template<size_t Sz, size_t MaxSz>
		constexpr void validate_size( ) {
			static_assert( Sz <= MaxSz,
			               "Size of function is larger than allocated space" );
		}
	} // namespace func_impl

	template<size_t, typename>
	class function;

	template<size_t MaxSize, typename Result, typename... FuncArgs>
	class function<MaxSize, Result( FuncArgs... )> {
		using function_base = func_impl::function_base<Result, FuncArgs...>;
		using empty_child = func_impl::empty_child<Result, FuncArgs...>;

		template<typename Func>
		using function_child = func_impl::function_child<Func, Result, FuncArgs...>;

		func_impl::function_storage<MaxSize, function_base> m_storage =
		  empty_child{ };

		template<typename Func>
		decltype( auto ) store_if_not_empty( Func &&f ) {
			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			return func_impl::store_if_not_empty<MaxSize, function_base,
			                                     function_child<Func>, empty_child>(
			  DAW_FWD( f ) );
		}

	public:
		explicit function( ) = default;

		explicit function( std::nullptr_t ) {}

		template<size_t N,
		         std::enable_if_t<( N <= MaxSize ), std::nullptr_t> = nullptr>
		function( function<N, Result( FuncArgs... )> const &other )
		  : m_storage( *other.m_storage ) {}

		template<size_t N,
		         std::enable_if_t<( N > MaxSize ), std::nullptr_t> = nullptr>
		function( function<N, Result( FuncArgs... )> const &other ) = delete;

		template<size_t N,
		         std::enable_if_t<( N <= MaxSize ), std::nullptr_t> = nullptr>
		function &operator=( function<N, Result( FuncArgs... )> const &other ) {
			m_storage = *other.m_storage;
			return *this;
		}

		template<size_t N,
		         std::enable_if_t<( N > MaxSize ), std::nullptr_t> = nullptr>
		function &
		operator=( function<N, Result( FuncArgs... )> const &other ) = delete;

		template<typename Func,
		         std::enable_if_t<
		           daw::all_true_v<!std::is_same_v<std::decay_t<Func>, function>,
		                           !std::is_function_v<Func>>,
		           std::nullptr_t> = nullptr>
		function( Func &&f )
		  : m_storage( store_if_not_empty( DAW_FWD( f ) ) ) {
			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			static_assert( std::is_invocable_r_v<Result, Func, FuncArgs...>,
			               "Function isn't callable with FuncArgs" );
		}

		template<typename Func,
		         std::enable_if_t<
		           daw::all_true_v<!std::is_same_v<std::decay_t<Func>, function>,
		                           !std::is_function_v<Func>>,
		           std::nullptr_t> = nullptr>
		function &operator=( Func &&f ) {
			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			static_assert( std::is_invocable_r_v<Result, Func, FuncArgs...>,
			               "Function isn't callable with FuncArgs" );
			m_storage = store_if_not_empty( DAW_FWD( f ) );
			return *this;
		}

		template<typename... Args>
		Result operator( )( Args &&...args ) {
			daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
			function_base &f = *m_storage;
			if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
				f( DAW_FWD( args )... );
				return;
			} else {
				return f( DAW_FWD( args )... );
			}
		}

		template<typename... Args>
		Result operator( )( Args &&...args ) const {
			daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
			function_base const &f = *m_storage;
			if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
				f( DAW_FWD( args )... );
				return;
			} else {
				return f( DAW_FWD( args )... );
			}
		}

		[[nodiscard]] bool empty( ) const {
			return m_storage->empty( );
		}

		explicit operator bool( ) const {
			return !empty( );
		}
	};
} // namespace daw

#if defined( DAW_HAS_GCC )
#pragma GCC diagnostic pop
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>
#include <concepts>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>

#ifdef DAW_CPP26_CONSTEXPR
#undef DAW_CPP26_CONSTEXPR
#endif
#ifdef DAW_CPP26_CONSTEVAL
#undef DAW_CPP26_CONSTEVAL
#endif
#if __cpp_constexpr >= 202306L
#define DAW_CPP26_CONSTEXPR constexpr
#define DAW_CPP26_CONSTEVAL consteval
#else
#define DAW_CPP26_CONSTEXPR
#define DAW_CPP26_CONSTEVAL
#endif

namespace daw {
	namespace mutable_function_ref_details {
		template<typename Ret, typename T, typename... Ps>
		concept class_invocable_r = std::is_class_v<std::remove_cvref_t<T>> and
		                            std::is_invocable_r_v<Ret, T, Ps...>;

		template<typename T, typename U>
		concept different_from = not std::same_as<std::remove_cvref_t<T>, U>;
	} // namespace mutable_function_ref_details

	template<typename>
	class mutable_function_ref;

	template<typename Result, typename... Params>
	class mutable_function_ref<Result( Params... )> {
		union data_t {
			void *obj_ptr;
			void const *const_obj_ptr;
			Result ( *func_ptr )( Params... );

			constexpr data_t( void *ptr ) noexcept
			  : obj_ptr( ptr ) {}

			constexpr data_t( void const *ptr ) noexcept
			  : const_obj_ptr( ptr ) {}

			constexpr data_t( Result ( *ptr )( Params... ) ) noexcept
			  : func_ptr( ptr ) {}
		};

		data_t m_data;
		Result ( *m_thunk )( data_t const &, Params... );

		template<typename Func>
		static DAW_CPP26_CONSTEXPR Result obj_thunk( data_t const &d,
		                                             Params... params ) {
			auto &obj = *static_cast<Func *>( d.obj_ptr );
			return obj( std::forward<Params>( params )... );
		}

		template<typename Func>
		static DAW_CPP26_CONSTEXPR Result const_obj_thunk( data_t const &d,
		                                                   Params... params ) {
			auto const &obj = *static_cast<Func const *>( d.const_obj_ptr );
			return obj( std::forward<Params>( params )... );
		}

		static constexpr Result fp_thunk( data_t const &d, Params... params ) {
			return d.func_ptr( params... );
		}

		struct empty_call {
			explicit empty_call( ) = default;
			[[noreturn]] Result operator( )( Params... ) const {
				std::abort( );
			}
		};

	public:
		constexpr mutable_function_ref( ) noexcept
		  : mutable_function_ref( empty_call{ } ) {}

		template<typename Func>
		requires( mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<Result, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref( Func &func ) noexcept
		  : m_data( static_cast<void *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<typename Func>
		requires(
		  mutable_function_ref_details::different_from<Func, mutable_function_ref>
		    and mutable_function_ref_details::class_invocable_r<
		      Result, Func const &, Params...> ) //
		  constexpr mutable_function_ref( Func const &func ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( func ) ) )
		  , m_thunk( const_obj_thunk<Func> ) {}

		template<typename Func>
		requires( not std::is_lvalue_reference_v<Func> and
		          mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<Result, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref( Func &&func ) noexcept
		  : m_data( static_cast<void *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<typename Func>
		requires( mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<Result, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref &operator=( Func &func ) noexcept {
			m_data = static_cast<void *>( std::addressof( func ) );
			m_thunk = obj_thunk<Func>;
			return *this;
		}

		template<typename Func>
		requires(
		  mutable_function_ref_details::different_from<Func, mutable_function_ref>
		    and mutable_function_ref_details::class_invocable_r<
		      Result, Func const &, Params...> ) //
		  constexpr mutable_function_ref &operator=( Func const &func ) noexcept {
			m_data = static_cast<void const *>( std::addressof( func ) );
			m_thunk = const_obj_thunk<Func>;
			return *this;
		}

		constexpr mutable_function_ref( Result ( *ptr )( Params... ) ) noexcept
		  : m_data( ptr )
		  , m_thunk( nullptr ) {
			assert( ptr );
		}

		constexpr mutable_function_ref &
		operator=( Result ( *ptr )( Params... ) ) noexcept {
			m_data = ptr;
			m_thunk = nullptr;
			assert( ptr );
			return *this;
		}

		constexpr Result operator( )( Params... params ) const {
			if( not m_thunk ) {
				return m_data.func_ptr( std::forward<Params>( params )... );
			}
			return m_thunk( m_data, std::forward<Params>( params )... );
		}
	};

	template<typename... Params>
	class mutable_function_ref<void( Params... )> {
		union data_t {
			void *obj_ptr;
			void const *const_obj_ptr;
			void ( *func_ptr )( Params... );

			constexpr data_t( void *ptr ) noexcept
			  : obj_ptr( ptr ) {}

			constexpr data_t( void const *ptr ) noexcept
			  : const_obj_ptr( ptr ) {}

			constexpr data_t( void ( *ptr )( Params... ) ) noexcept
			  : func_ptr( ptr ) {}
		};

		data_t m_data;
		void ( *m_thunk )( data_t const &, Params... );

		template<typename Func>
		static DAW_CPP26_CONSTEXPR void obj_thunk( data_t const &d,
		                                           Params... params ) {
			auto &obj = *static_cast<Func *>( d.obj_ptr );
			(void)obj( std::forward<Params>( params )... );
		}

		template<typename Func>
		static DAW_CPP26_CONSTEXPR void const_obj_thunk( data_t const &d,
		                                                 Params... params ) {
			auto const &obj = *static_cast<Func const *>( d.const_obj_ptr );
			(void)obj( std::forward<Params>( params )... );
		}

		static constexpr void fp_thunk( data_t const &d, Params... params ) {
			d.func_ptr( params... );
		}

	public:
		template<typename Func>
		requires( mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<void, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref( Func &func ) noexcept
		  : m_data( static_cast<void *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<typename Func>
		requires(
		  mutable_function_ref_details::different_from<Func, mutable_function_ref>
		    and mutable_function_ref_details::class_invocable_r<void, Func const &,
		                                                        Params...> ) //
		  constexpr mutable_function_ref( Func const &func ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( func ) ) )
		  , m_thunk( const_obj_thunk<Func> ) {}

		template<typename Func>
		requires( not std::is_lvalue_reference_v<Func> and
		          mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<void, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref( Func &&func ) noexcept
		  : m_data( static_cast<void *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<typename Func>
		requires( mutable_function_ref_details::different_from<
		            Func, mutable_function_ref> and
		          not std::is_const_v<Func> and
		          mutable_function_ref_details::class_invocable_r<void, Func &,
		                                                          Params...> ) //
		  constexpr mutable_function_ref &operator=( Func &func ) noexcept {
			m_data = static_cast<void *>( std::addressof( func ) );
			m_thunk = obj_thunk<Func>;
			return *this;
		}

		template<typename Func>
		requires(
		  mutable_function_ref_details::different_from<Func, mutable_function_ref>
		    and mutable_function_ref_details::class_invocable_r<void, Func const &,
		                                                        Params...> ) //
		  constexpr mutable_function_ref &operator=( Func const &func ) noexcept {
			m_data = static_cast<void const *>( std::addressof( func ) );
			m_thunk = const_obj_thunk<Func>;
			return *this;
		}

		constexpr mutable_function_ref( void ( *ptr )( Params... ) ) noexcept
		  : m_data( ptr )
		  , m_thunk( fp_thunk ) {
			assert( ptr );
		}

		constexpr mutable_function_ref &
		operator=( void ( *ptr )( Params... ) ) noexcept {
			m_data = ptr;
			m_thunk = fp_thunk;
			assert( ptr );
			return *this;
		}

		constexpr void operator( )( Params... params ) const {
			m_thunk( m_data, params... );
		}
	};
} // namespace daw

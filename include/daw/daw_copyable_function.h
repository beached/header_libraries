// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_ensure.h"
#include "daw/daw_move.h"
#include "daw/daw_unique_ptr.h"

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename Signature>
	class copyable_function;
}

namespace daw::copyable_function_details {
	enum class cvref_t {
		none,
		constant,
		lvalue,
		const_lvalue,
		rvalue,
		const_rvalue
	};

	template<typename T>
	inline constexpr bool is_copyable_function_v = false;

	template<typename Signature>
	inline constexpr bool is_copyable_function_v<copyable_function<Signature>> =
	  true;

	template<typename T>
	struct is_in_place_type : std::false_type {};

	template<typename T>
	struct is_in_place_type<std::in_place_type_t<T>> : std::true_type {};

	template<typename R, typename Fn, typename... Args>
	constexpr R invoke_r( Fn &&fn, Args &&...args ) noexcept(
	  std::is_nothrow_invocable_r_v<R, Fn, Args...> ) {
		if constexpr( std::is_void_v<R> ) {
			std::invoke( DAW_FWD( fn ), DAW_FWD( args )... );
		} else {
			return std::invoke( DAW_FWD( fn ), DAW_FWD( args )... );
		}
	}

	template<cvref_t CVRef, typename T>
	struct qualified_types;

	template<typename T>
	struct qualified_types<cvref_t::none, T> {
		using signature_type = T;
		using invocation_type = T &;
	};

	template<typename T>
	struct qualified_types<cvref_t::constant, T> {
		using signature_type = T const;
		using invocation_type = T const &;
	};

	template<typename T>
	struct qualified_types<cvref_t::lvalue, T> {
		using signature_type = T &;
		using invocation_type = T &;
	};

	template<typename T>
	struct qualified_types<cvref_t::const_lvalue, T> {
		using signature_type = T const &;
		using invocation_type = T const &;
	};

	template<typename T>
	struct qualified_types<cvref_t::rvalue, T> {
		using signature_type = T &&;
		using invocation_type = T &&;
	};

	template<typename T>
	struct qualified_types<cvref_t::const_rvalue, T> {
		using signature_type = T const &&;
		using invocation_type = T const &&;
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename Fn,
	         typename... Args>
	inline constexpr bool is_callable_from_v = [] {
		using qualified = qualified_types<CVRef, Fn>;
		if constexpr( IsNoExcept ) {
			return std::is_nothrow_invocable_r_v<R,
			                                     typename qualified::signature_type,
			                                     Args...> and
			       std::is_nothrow_invocable_r_v<R,
			                                     typename qualified::invocation_type,
			                                     Args...>;
		} else {
			return std::is_invocable_r_v<R,
			                             typename qualified::signature_type,
			                             Args...> and
			       std::is_invocable_r_v<R,
			                             typename qualified::invocation_type,
			                             Args...>;
		}
	}( );

	template<cvref_t CVRef, typename Self>
	inline constexpr bool valid_self_v = [] {
		using object_t = std::remove_reference_t<Self>;
		if constexpr( std::is_volatile_v<object_t> ) {
			return false;
		} else if constexpr( CVRef == cvref_t::none ) {
			return not std::is_const_v<object_t>;
		} else if constexpr( CVRef == cvref_t::constant ) {
			return true;
		} else if constexpr( CVRef == cvref_t::lvalue ) {
			return not std::is_const_v<object_t> and std::is_lvalue_reference_v<Self>;
		} else if constexpr( CVRef == cvref_t::const_lvalue ) {
			return std::is_lvalue_reference_v<Self>;
		} else if constexpr( CVRef == cvref_t::rvalue ) {
			return not std::is_const_v<object_t> and std::is_rvalue_reference_v<Self>;
		} else {
			return std::is_rvalue_reference_v<Self>;
		}
	}( );

	template<typename Fn, typename Signature, cvref_t CVRef, bool IsNoExcept,
	         typename R, typename... Args>
	concept compatible_function = requires {
		requires not std::is_same_v<std::remove_cvref_t<Fn>,
		                            copyable_function<Signature>>;
		requires not is_in_place_type<std::remove_cvref_t<Fn>>::value;
		requires is_callable_from_v<CVRef, IsNoExcept, R, std::decay_t<Fn>,
		                            Args...>;
		requires std::is_constructible_v<std::decay_t<Fn>, Fn>;
		requires std::is_copy_constructible_v<std::decay_t<Fn>>;
	};

	template<typename T, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Args>
	concept compatible_in_place_function =
	  std::is_same_v<T, std::decay_t<T>> and std::is_copy_constructible_v<T> and
	  is_callable_from_v<CVRef, IsNoExcept, R, T, Args...>;

	template<typename T, typename... ConstructorArgs>
	concept constructible_function =
	  std::is_constructible_v<T, ConstructorArgs...>;

	template<typename T, typename U, typename... ConstructorArgs>
	concept initializer_list_constructible_function =
	  std::is_constructible_v<T, std::initializer_list<U> &, ConstructorArgs...>;

	template<cvref_t CVRef, typename Self>
	concept compatible_function_self = valid_self_v<CVRef, Self>;

	template<typename Function, typename Base, typename Fn>
	concept assignable_function = requires( Base &base, Fn &&fn ) {
		requires not std::is_same_v<std::remove_cvref_t<Fn>, Function>;
		base.operator=( DAW_FWD( fn ) );
	};

	template<typename Fn>
	constexpr bool is_null_target( Fn const &fn ) noexcept {
		if constexpr( std::is_pointer_v<Fn> or std::is_member_pointer_v<Fn> ) {
			return fn == nullptr;
		} else if constexpr( is_copyable_function_v<Fn> ) {
			return not static_cast<bool>( fn );
		} else {
			return false;
		}
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Args>
	class storage_base {
	public:
		constexpr virtual ~storage_base( ) = default;

		[[nodiscard]] virtual constexpr unique_ptr<storage_base> clone( ) const = 0;

		virtual constexpr R call( Args... ) noexcept( IsNoExcept ) = 0;
	};

	template<typename Fn, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Args>
	class storage final : public storage_base<CVRef, IsNoExcept, R, Args...> {
		using base_t = storage_base<CVRef, IsNoExcept, R, Args...>;
		Fn m_function;

	public:
		template<typename... ConstructorArgs>
		explicit constexpr storage( std::in_place_t, ConstructorArgs &&...args )
		  : m_function( DAW_FWD( args )... ) {}

		template<typename U, typename... ConstructorArgs>
		explicit constexpr storage( std::in_place_t,
		                            std::initializer_list<U> values,
		                            ConstructorArgs &&...args )
		  : m_function( values, DAW_FWD( args )... ) {}

		[[nodiscard]] constexpr unique_ptr<base_t> clone( ) const override {
			return unique_ptr<base_t>( new storage( std::in_place, m_function ) );
		}

		constexpr R call( Args... args ) noexcept( IsNoExcept ) override {
			if constexpr( CVRef == cvref_t::constant or
			              CVRef == cvref_t::const_lvalue ) {
				return invoke_r<R>( std::as_const( m_function ),
				                    std::forward<Args>( args )... );
			} else if constexpr( CVRef == cvref_t::rvalue ) {
				return invoke_r<R>( std::move( m_function ),
				                    std::forward<Args>( args )... );
			} else if constexpr( CVRef == cvref_t::const_rvalue ) {
				return invoke_r<R>( std::move( std::as_const( m_function ) ),
				                    std::forward<Args>( args )... );
			} else {
				return invoke_r<R>( m_function, std::forward<Args>( args )... );
			}
		}
	};

	template<typename Signature, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Args>
	class function_impl {
		using storage_base_t = storage_base<CVRef, IsNoExcept, R, Args...>;
		unique_ptr<storage_base_t> m_storage;

		template<typename Fn, typename... ConstructorArgs>
		constexpr void emplace( ConstructorArgs &&...args ) {
			using storage_t = storage<Fn, CVRef, IsNoExcept, R, Args...>;
			m_storage = unique_ptr<storage_base_t>(
			  new storage_t( std::in_place, DAW_FWD( args )... ) );
		}

	public:
		using result_type = R;

		constexpr function_impl( ) noexcept = default;
		constexpr function_impl( std::nullptr_t ) noexcept {}

		template<typename Fn>
		requires compatible_function<Fn, Signature, CVRef, IsNoExcept, R, Args...>
		constexpr function_impl( Fn &&fn ) {
			using function_t = std::decay_t<Fn>;
			if( not is_null_target<function_t>( fn ) ) {
				emplace<function_t>( DAW_FWD( fn ) );
			}
		}

		template<typename T, typename... ConstructorArgs>
		requires( compatible_in_place_function<T, CVRef, IsNoExcept, R, Args...> and
		          constructible_function<T, ConstructorArgs...> )
		explicit constexpr function_impl( std::in_place_type_t<T>,
		                                  ConstructorArgs &&...args ) {
			emplace<T>( DAW_FWD( args )... );
		}

		template<typename T, typename U, typename... ConstructorArgs>
		requires(
		  compatible_in_place_function<T, CVRef, IsNoExcept, R, Args...> and
		  initializer_list_constructible_function<T, U, ConstructorArgs...> )
		explicit constexpr function_impl( std::in_place_type_t<T>,
		                                  std::initializer_list<U> values,
		                                  ConstructorArgs &&...args ) {
			emplace<T>( values, DAW_FWD( args )... );
		}

		constexpr function_impl( function_impl const &other )
		  : m_storage( other.m_storage ? other.m_storage->clone( ) : nullptr ) {}

		constexpr function_impl( function_impl && ) noexcept = default;

		constexpr function_impl &operator=( function_impl const &rhs ) {
			if( this != &rhs ) {
				function_impl( rhs ).swap( *this );
			}
			return *this;
		}

		constexpr function_impl &operator=( function_impl && ) = default;

		constexpr function_impl &operator=( std::nullptr_t ) noexcept {
			m_storage.reset( );
			return *this;
		}

		template<typename Fn>
		requires compatible_function<Fn, Signature, CVRef, IsNoExcept, R, Args...>
		constexpr function_impl &operator=( Fn &&fn ) {
			function_impl( DAW_FWD( fn ) ).swap( *this );
			return *this;
		}

		template<typename Self>
		requires compatible_function_self<CVRef, Self &&>
		constexpr R operator( )( this Self &&self,
		                         Args... args ) noexcept( IsNoExcept ) {
			daw_dbg_ensure( self.m_storage );
			return self.m_storage->call( std::forward<Args>( args )... );
		}

		constexpr explicit operator bool( ) const noexcept {
			return static_cast<bool>( m_storage );
		}

		constexpr void swap( function_impl &other ) noexcept {
			m_storage.swap( other.m_storage );
		}
	};

	template<typename Signature, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Args>
	struct signature_traits_base {
		using implementation =
		  function_impl<Signature, CVRef, IsNoExcept, R, Args...>;
	};

	template<typename Signature>
	struct signature_traits;

	template<typename R, typename... Args>
	struct signature_traits<R( Args... )>
	  : signature_traits_base<R( Args... ), cvref_t::none, false, R, Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const>
	  : signature_traits_base<R( Args... ) const, cvref_t::constant, false, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) &>
	  : signature_traits_base<R( Args... ) &, cvref_t::lvalue, false, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const &>
	  : signature_traits_base<R( Args... ) const &, cvref_t::const_lvalue, false,
	                          R, Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) &&>
	  : signature_traits_base<R( Args... ) &&, cvref_t::rvalue, false, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const &&>
	  : signature_traits_base<R( Args... ) const &&, cvref_t::const_rvalue, false,
	                          R, Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) noexcept>
	  : signature_traits_base<R( Args... ) noexcept, cvref_t::none, true, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const noexcept>
	  : signature_traits_base<R( Args... ) const noexcept, cvref_t::constant,
	                          true, R, Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) & noexcept>
	  : signature_traits_base<R( Args... ) & noexcept, cvref_t::lvalue, true, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const & noexcept>
	  : signature_traits_base<R( Args... ) const & noexcept,
	                          cvref_t::const_lvalue, true, R, Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) && noexcept>
	  : signature_traits_base<R( Args... ) && noexcept, cvref_t::rvalue, true, R,
	                          Args...> {};

	template<typename R, typename... Args>
	struct signature_traits<R( Args... ) const && noexcept>
	  : signature_traits_base<R( Args... ) const && noexcept,
	                          cvref_t::const_rvalue, true, R, Args...> {};
} // namespace daw::copyable_function_details

namespace daw {
	template<typename Signature>
	class copyable_function : public copyable_function_details::signature_traits<
	                            Signature>::implementation {
		using base_t = typename copyable_function_details::signature_traits<
		  Signature>::implementation;

	public:
		using base_t::base_t;

		constexpr copyable_function( ) noexcept = default;
		constexpr copyable_function( copyable_function const & ) = default;
		constexpr copyable_function( copyable_function && ) noexcept = default;
		constexpr copyable_function &
		operator=( copyable_function const & ) = default;
		constexpr copyable_function &operator=( copyable_function && ) = default;
		constexpr ~copyable_function( ) = default;

		constexpr copyable_function &operator=( std::nullptr_t ) noexcept {
			base_t::operator=( nullptr );
			return *this;
		}

		template<typename Fn>
		requires copyable_function_details::assignable_function<copyable_function,
		                                                        base_t, Fn>
		constexpr copyable_function &operator=( Fn &&fn ) {
			base_t::operator=( DAW_FWD( fn ) );
			return *this;
		}

		constexpr void swap( copyable_function &other ) noexcept {
			base_t::swap( other );
		}

		friend constexpr void swap( copyable_function &lhs,
		                            copyable_function &rhs ) noexcept {
			lhs.swap( rhs );
		}

		friend constexpr bool operator==( copyable_function const &function,
		                                  std::nullptr_t ) noexcept {
			return not static_cast<bool>( function );
		}
	};
} // namespace daw

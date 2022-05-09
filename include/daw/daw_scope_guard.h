// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"
#include "daw_exchange.h"
#include "daw_is_constant_evaluated.h"
#include "daw_move.h"

#include <ciso646>
#include <exception>
#include <optional>
#include <type_traits>

#if defined( __cpp_constexpr_dynamic_alloc ) and                               \
  defined( DAW_IS_CONSTANT_EVALUATED )
#define DAW_SG_CXDTOR constexpr

/***
 * Scope guard will execute the handler in a constant expression
 */
#define DAW_HAS_CONSTEXPR_SCOPE_GUARD
#else
#define DAW_SG_CXDTOR
#endif

namespace daw {
	template<typename FunctionType>
	class ScopeGuard {
		std::optional<FunctionType> m_function;

	public:
		ScopeGuard( ) = delete;
		ScopeGuard( const ScopeGuard & ) = delete;
		ScopeGuard &operator=( const ScopeGuard & ) = delete;

		constexpr ScopeGuard( FunctionType f ) noexcept(
		  std::is_nothrow_move_constructible_v<FunctionType> )
		  : m_function{ DAW_MOVE( f ) } {}

		constexpr ScopeGuard( ScopeGuard &&other ) noexcept(
		  std::is_nothrow_move_constructible_v<FunctionType> )
		  : m_function{ std::exchange( other.m_function, std::nullopt ) } {}

		constexpr ScopeGuard &operator=( ScopeGuard &&rhs ) noexcept(
		  std::is_nothrow_move_constructible_v<FunctionType> ) {
			m_function = std::exchange( rhs.m_function, std::nullopt );
			return *this;
		}

		DAW_SG_CXDTOR ~ScopeGuard( ) noexcept(
		  std::is_nothrow_invocable_v<FunctionType> ) {
			if( m_function ) {
				(*m_function)( );
			}
		}

		constexpr void dismiss( ) const noexcept {
			m_function = std::optional<FunctionType>( );
		}

		[[nodiscard]] constexpr bool
		operator==( const ScopeGuard &rhs ) const noexcept {
			return rhs.m_function == m_function;
		}
	}; // class ScopeGuard

	template<typename FunctionType>
	[[nodiscard]] constexpr ScopeGuard<FunctionType>
	on_scope_exit( FunctionType f ) noexcept(
	  std::is_nothrow_move_constructible_v<FunctionType> ) {
		return ScopeGuard<FunctionType>( DAW_MOVE( f ) );
	}

	template<typename Handler>
	class on_exit_success {
		Handler on_exit_handler;

	public:
		constexpr on_exit_success( Handler &&h ) noexcept(
		  std::is_nothrow_move_constructible_v<Handler> )
		  : on_exit_handler( DAW_MOVE( h ) ) {}

		constexpr on_exit_success( Handler const &h ) noexcept(
		  std::is_nothrow_copy_constructible_v<Handler> )
		  : on_exit_handler( h ) {}

		DAW_SG_CXDTOR ~on_exit_success( ) noexcept(
		  std::is_nothrow_invocable_v<Handler> ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
			if( DAW_IS_CONSTANT_EVALUATED( ) ) {
				on_exit_handler( );
			} else {
#endif
#if not defined( DAW_DONT_USE_EXCEPTIONS )
				if( std::uncaught_exceptions( ) == 0 ) {
#endif
					on_exit_handler( );
#if not defined( DAW_DONT_USE_EXCEPTIONS )
				}
#endif
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
			}
#endif
		}
	};

	template<typename Handler>
	on_exit_success( Handler ) -> on_exit_success<Handler>;
} // namespace daw

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
#include "daw_swap.h"
#include "daw_traits.h"

#include <ciso646>
#include <exception>
#include <utility>

#if defined( __cpp_constexpr_dynamic_alloc ) and                               \
  defined( DAW_IS_CONSTANT_EVALUATED )

#define CXDTOR constexpr
#define CXEVAL DAW_IS_CONSTANT_EVALUATED( )
#define HAS_CXSTOR_AND_EVAL

#else

#define CXDTOR
#define CXEVAL

#endif

namespace daw {
	template<typename FunctionType>
	class ScopeGuard {
		FunctionType m_function;
		mutable bool m_is_active;

	public:
		ScopeGuard( ) = delete;
		ScopeGuard( const ScopeGuard & ) = delete;
		ScopeGuard &operator=( const ScopeGuard & ) = delete;

		constexpr ScopeGuard( FunctionType f ) noexcept
		  : m_function{ daw::move( f ) }
		  , m_is_active{ true } {}

		constexpr ScopeGuard( ScopeGuard &&other ) noexcept
		  : m_function{ daw::move( other.m_function ) }
		  , m_is_active{ daw::exchange( other.m_is_active, false ) } {}

		constexpr ScopeGuard &operator=( ScopeGuard &&rhs ) noexcept {
			m_function = daw::move( rhs.m_function );
			m_is_active = daw::exchange( rhs.m_is_active, false );
			return *this;
		}

		CXDTOR ~ScopeGuard( ) noexcept {
			if( m_is_active ) {
				m_function( );
			}
		}

		constexpr void dismiss( ) const noexcept {
			m_function = nullptr;
			m_is_active = false;
		}

		[[nodiscard]] constexpr bool
		operator==( const ScopeGuard &rhs ) const noexcept {
			return rhs.m_function == m_function and rhs.m_is_active == m_is_active;
		}
	}; // class ScopeGuard

	template<typename FunctionType>
	[[nodiscard]] constexpr ScopeGuard<FunctionType>
	on_scope_exit( FunctionType f ) noexcept {
		return ScopeGuard<FunctionType>( daw::move( f ) );
	}

	template<typename Handler>
	class on_exit_success {
		Handler on_exit_handler;

	public:
		constexpr on_exit_success( Handler &&h ) noexcept(
		  std::is_nothrow_move_constructible_v<Handler> )
		  : on_exit_handler( std::move( h ) ) {}

		constexpr on_exit_success( Handler const &h ) noexcept(
		  std::is_nothrow_copy_constructible_v<Handler> )
		  : on_exit_handler( h ) {}

		CXDTOR ~on_exit_success( ) noexcept( noexcept( on_exit_handler( ) ) ) {
#if defined( HAS_CXSTOR_AND_EVAL )
			if( CXEVAL ) {
				on_exit_handler( );
			} else {
#endif
				if( std::uncaught_exceptions( ) == 0 ) {
					on_exit_handler( );
				}
#if defined( HAS_CXSTOR_AND_EVAL )
			}
#endif
		}
	};

	template<typename Handler>
	on_exit_success( Handler ) -> on_exit_success<Handler>;
} // namespace daw

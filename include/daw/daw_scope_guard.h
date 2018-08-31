// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <utility>

#include "daw_traits.h"

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
		  : m_function{std::move( f )}
		  , m_is_active{true} {}

		constexpr ScopeGuard( ScopeGuard &&other ) noexcept
		  : m_function{std::move( other.m_function )}
		  , m_is_active{daw::exchange( other.m_is_active, false )} {}

		constexpr ScopeGuard &operator=( ScopeGuard &&rhs ) noexcept {
			m_function = std::move( rhs.m_function );
			m_is_active = daw::exchange( rhs.m_is_active, false );
			return *this;
		}

#if !defined( MAY_THROW_EXCEPTIONS ) || MAY_THROW_EXCEPTIONS
		~ScopeGuard( ) noexcept {
			if( m_is_active ) {
				try {
					m_function( );
				} catch( ... ) { std::terminate( ); }
			}
		}
#else
		~ScopeGuard( ) noexcept {
			if( m_is_active ) {
				m_function( );
			}
		}
#endif
		constexpr void dismiss( ) const noexcept {
			m_function = nullptr;
			m_is_active = false;
		}

		constexpr bool operator==( const ScopeGuard &rhs ) const noexcept {
			return rhs.m_function == m_function && rhs.m_is_active == m_is_active;
		}
	}; // class ScopeGuard

	template<typename FunctionType>
	constexpr ScopeGuard<FunctionType> on_scope_exit( FunctionType f ) noexcept {
		return ScopeGuard<FunctionType>( std::move( f ) );
	}
} // namespace daw

// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <functional>

namespace daw {
	template<typename FunctionType>
	class ScopeGuard {
		FunctionType m_function;
		mutable bool m_is_active;

	public:
		ScopeGuard( FunctionType f ) : m_function( std::move( f ) ), m_is_active( true ) {}

		~ScopeGuard( ) {
			if( m_is_active ) {
				m_function( );
			}
		}

		void dismiss( ) const {
			m_is_active = false;
		}

		ScopeGuard( ) = delete;
		ScopeGuard( const ScopeGuard & ) = delete;

		ScopeGuard( ScopeGuard &&other )
		  : m_function( std::move( other.m_function ) ), m_is_active( std::move( other.m_is_active ) ) {
			other.dismiss( );
		}

		ScopeGuard &operator=( ScopeGuard rhs ) {
			m_function = std::move( rhs.m_function );
			m_is_active = std::move( rhs.m_is_active );
			rhs.dismiss( );
			return *this;
		}

		bool operator==( const ScopeGuard &rhs ) const {
			return rhs.m_function == m_function && rhs.m_is_active == m_is_active;
		}
	}; // class ScopeGuard

	template<typename FunctionType>
	ScopeGuard<FunctionType> on_scope_exit( FunctionType f ) {
		return ScopeGuard<FunctionType>( std::move( f ) );
	}

	namespace detail {
		enum class ScopeGuardOnExit {};

		template<typename FunctionType>
		ScopeGuard<FunctionType> operator+( ScopeGuardOnExit, FunctionType &&fn ) {
			return ScopeGuard<FunctionType>( std::forward<FunctionType>( fn ) );
		}
	} // namespace detail

#define SCOPE_EXIT auto ANONYMOUS_VARIABLE( SCOPE_EXIT_STATE ) = ::daw::detail::ScopeGuardOnExit( ) + [&]( )

#define CONCATENATE_IMPL( s1, s2 ) s1##s2
#define CONCATENATE( s1, s2 ) CONCATENATE_IMPL( s1, s2 )
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE( str ) CONCATENATE( str, __COUNTER__ )
#else
#define ANONYMOUS_VARIABLE( str ) CONCATENATE( str, __LINE__ )
#endif
} // namespace daw

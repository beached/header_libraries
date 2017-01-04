#pragma once
// The MIT License ( MIT )
//
// Copyright ( c ) 2013-2016 Darrell Wright
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

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "daw_optional.h"
#include "daw_traits.h"
#include "daw_exception.h"

namespace daw {
	template<class ValueType>
	class expected_t {
		daw::optional<ValueType> m_value;
		std::exception_ptr m_exception;
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) : 
				m_value{ }, 
				m_exception{ } { }

		expected_t( expected_t const & ) = default;
		expected_t& operator=( expected_t const & ) = default;
		expected_t( expected_t &&  ) = default;
		expected_t& operator=( expected_t && ) = default;
		~expected_t( ) = default;

		friend bool operator==( expected_t const & lhs, expected_t const & rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) == std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t( ValueType value ) noexcept: 
				m_value{ std::move( value ) }, 
				m_exception{ } { }

		expected_t & operator=( ValueType value ) noexcept {
			m_value = std::move( value );
			m_exception = std::exception_ptr{ };
			return *this;
		}

		auto & from_value( ValueType value ) noexcept {
			m_value = std::move( value );
			m_exception = std::exception_ptr{ };
			return *this;
		}

		template<class ExceptionType>
		auto & from_exception( ExceptionType const & exception ) {
			if( typeid( exception ) != typeid( ExceptionType ) ) {
				throw std::invalid_argument( "slicing detected" );
			}
			return from_exception( std::make_exception_ptr( exception ) );
		}

		auto & from_exception( std::exception_ptr ptr ) {
			expected_t result;
			new( &result.m_exception ) std::exception_ptr{ std::move( ptr ) };
			using std::swap;
			swap( *this, result );
			return *this;
		}

		auto & from_exception( ) {
			return from_exception( std::current_exception( ) );
		}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		auto & get( ) {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			daw::exception::daw_throw_on_false( has_value( ) );	
			return *m_value;
		}

		auto const & get( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			daw::exception::daw_throw_on_false( has_value( ) );	
			return *m_value;
		}

		std::string get_exception_message( ) const {
			try {
				if( m_exception ) {
					std::rethrow_exception( m_exception );
				}
			} catch( std::exception const & e ) {
				return std::string{ e.what( ) };
			}
			return { }; 
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		bool empty( ) const {
			return !(static_cast<bool>( m_value ) || static_cast<bool>( m_exception ));
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		template<class Function, typename... Args>
		auto from_code( Function func, Args&&... args ) {
			try {
				return from_value( func( std::forward<Args>( args )... ) );
			} catch( ... ) {
				return from_exception( );
			}
		}
	};	// class expected_t
	static_assert( daw::traits::is_regular<expected_t<int>>::value, "expected_t isn't regular" );
}	// namespace daw


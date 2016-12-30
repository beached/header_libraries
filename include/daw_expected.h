#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2016 Darrell Wright
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
#include "daw_traits.h"

namespace daw {
	template<class ValueType>
	class expected_t {
		enum class expected_tSource { value, exception, none };
		ValueType m_value;
		std::exception_ptr m_exception;
		expected_tSource m_source;
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) : 
				m_value{ }, 
				m_exception{ }, 
				m_source{ expected_tSource::none } { }

		expected_t( expected_t const & other ) = default;
		expected_t( expected_t && other ) = default;
		expected_t& operator=(expected_t const & rhs) = default;
		expected_t& operator=(expected_t && rhs) = default;
		~expected_t( ) = default;

		friend bool operator==(expected_t const & lhs, expected_t const & rhs) {
			return lhs.m_value == rhs.m_value && lhs.m_exception == rhs.m_exception && lhs.m_source == rhs.m_source;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t( ValueType value ) noexcept: 
				m_value{ std::move( value ) }, 
				m_exception{ },
				m_source{ expected_tSource::value } { }

		template<class ExceptionType>
		static expected_t<ValueType> from_exception( ExceptionType const & exception ) {
			if( typeid(exception) != typeid(ExceptionType) ) {
				throw std::invalid_argument( "slicing detected" );
			}
			return from_exception( std::make_exception_ptr( exception ) );
		}

		static expected_t<ValueType>
			from_exception( std::exception_ptr except_ptr ) {
			expected_t<ValueType> result;
			result.m_source = expected_t<ValueType>::expected_tSource::exception;
			new(&result.m_exception) std::exception_ptr( std::move( except_ptr ) );
			return result;
		}

		static expected_t<ValueType> from_exception( ) {
			return from_exception( std::current_exception( ) );
		}

		bool has_value( ) const noexcept {
			return expected_tSource::value == m_source;
		}

		ValueType & get( ) {
			assert( expected_tSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		ValueType const & get( ) const {
			assert( expected_tSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		ValueType move_out( ) {
			assert( expected_tSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			m_source = expected_tSource::none;
			return std::move( m_value );
		}

		std::string get_exception_message( ) const {
			std::string result;
			try {
				if( m_exception ) {
					std::rethrow_exception( m_exception );
				}
			} catch( std::exception const & e ) {
				result = e.what( );
			}
			return result;
		}

		bool has_exception( ) const noexcept {
			return expected_tSource::exception == m_source;
		}

		template<class FunctionType>
		static expected_t from_code( FunctionType func ) {
			try {
				return expected_t( func( ) );
			} catch( ... ) {
				return from_exception( );
			}
		}
	};	// class expected_t
	static_assert(::daw::traits::is_regular<expected_t<int>>::value, "expected_t isn't regular");
}	// namespace daw


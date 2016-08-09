#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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
	class Optional {
		enum class OptionalSource { value, exception, none };
		ValueType * m_value;
		std::exception_ptr m_exception;
		OptionalSource m_source;
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		Optional( ) : m_value( ), m_exception( ), m_source( OptionalSource::none ) { }

		Optional( Optional const & other ) = default;
		Optional& operator=(Optional const & rhs) = default;

		Optional( Optional&& other ) noexcept:
			m_value( std::move( other.m_value ) ),
			m_exception( std::move( other.m_exception ) ),
			m_source( std::move( other.m_source ) ) { }

		Optional& operator=(Optional && rhs) noexcept {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
				m_exception = std::move( rhs.m_exception );
				m_source = std::move( rhs.m_source );
			}
			return *this;
		}

		~Optional( ) = default;

		bool operator==(Optional const & rhs) const noexcept {
			return rhs.m_value == m_value && rhs.m_exception == m_exception && rhs.m_source == m_source;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		Optional( ValueType value ) noexcept: m_value( std::move( value ) ), m_exception( ), m_source( OptionalSource::value ) { }

		template<class ExceptionType>
		static Optional<ValueType> from_exception( ExceptionType const & exception ) {
			if( typeid(exception) != typeid(ExceptionType) ) {
				throw std::invalid_argument( "slicing detected" );
			}
			return from_exception( std::make_exception_ptr( exception ) );
		}

		static Optional<ValueType>
			from_exception( std::exception_ptr except_ptr ) {
			Optional<ValueType> result;
			result.m_source = Optional<ValueType>::OptionalSource::exception;
			new(&result.m_exception) std::exception_ptr( std::move( except_ptr ) );
			return result;
		}

		static Optional<ValueType> from_exception( ) {
			return from_exception( std::current_exception( ) );
		}

		bool has_value( ) const noexcept {
			return OptionalSource::value == m_source;
		}

		ValueType& get( ) {
			assert( OptionalSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		ValueType const & get( ) const {
			assert( OptionalSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		ValueType move_out( ) {
			assert( OptionalSource::none != m_source );
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			m_source = OptionalSource::none;
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
			return OptionalSource::exception == m_source;
		}

		template<class FunctionType>
		static Optional from_code( FunctionType func ) {
			try {
				return Optional( func( ) );
			} catch( ... ) {
				return from_exception( );
			}
		}
	};	// class Optional
	static_assert(::daw::traits::is_regular<Optional<int>>::value, "Optional isn't regular");
}	// namespace daw


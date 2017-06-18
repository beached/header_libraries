#pragma once
// The MIT License ( MIT )
//
// Copyright ( c ) 2013-2017 Darrell Wright
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

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_optional.h"
#include "daw_traits.h"

namespace daw {
	template<class T>
	struct expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;

		struct exception_tag {};

	  private:
		daw::optional<value_type> m_value;
		std::exception_ptr m_exception;

	  public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) = default;
		expected_t( expected_t const & ) = default;
		expected_t &operator=( expected_t const & ) = default;
		expected_t( expected_t && ) = default;
		expected_t &operator=( expected_t && ) = default;
		~expected_t( ) = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) == std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t( value_type value ) noexcept : m_value{std::move( value )}, m_exception{} {}

		expected_t &operator=( value_type value ) {
			using std::swap;
			expected_t tmp{std::move( value )};
			swap( *this, tmp );
			return *this;
		}

		expected_t( std::exception_ptr ptr ) : m_value{}, m_exception{std::move( ptr )} {}

		expected_t &operator=( std::exception_ptr ptr ) {
			using std::swap;
			expected_t tmp{std::move( ptr )};
			swap( *this, tmp );
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex ) : expected_t{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag ) : expected_t{std::current_exception( )} {}

//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function, Args...>>>
		template<class Function, typename... Args>
		static auto from_code( Function func, Args &&... args ) noexcept {
			using std::swap;
			try {
				return expected_t{func( std::forward<Args>( args )... )};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function, Args...>>>
		template<class Function, typename... Args>
		expected_t( Function func, Args &&... args ) noexcept
		    : expected_t{expected_t::from_code( func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		bool empty( ) const {
			return !( has_value( ) || has_exception( ) );
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		explicit operator value_type( ) const {
			return get( );
		}

		void throw_if_exception( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
		}

		reference get( ) {
			throw_if_exception( );
			return *m_value;
		}

		const_reference get( ) const {
			throw_if_exception( );
			return *m_value;
		}

		std::string get_exception_message( ) const {
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { return std::string{e.what( )}; } catch( ... ) {
				return {};
			}
			return {};
		}

	}; // class expected_t

	static_assert( daw::traits::is_regular<expected_t<int>>::value, "expected_t isn't regular" );

	template<>
	struct expected_t<void> {
		using value_type = void;

		struct exception_tag {};

	  private:
		bool m_value;
		std::exception_ptr m_exception;

		expected_t( bool b ) noexcept : m_value{b}, m_exception{} {}

	  public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( expected_t const & ) = default;
		expected_t &operator=( expected_t const & ) = default;
		expected_t( expected_t && ) = default;
		expected_t &operator=( expected_t && ) = default;
		~expected_t( ) = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) == std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////

		expected_t( ) noexcept : expected_t{false} {}

		template<typename T>
		expected_t( T ) noexcept : expected_t{true} {}

		template<typename T>
		expected_t &operator=( T ) {
			using std::swap;
			expected_t tmp{true};
			swap( *this, tmp );
			return *this;
		}

		expected_t( std::exception_ptr ptr ) : m_value{}, m_exception{std::move( ptr )} {}

		expected_t &operator=( std::exception_ptr ptr ) {
			using std::swap;
			expected_t tmp{std::move( ptr )};
			swap( *this, tmp );
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex ) : expected_t{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag ) : expected_t{std::current_exception( )} {}

//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function, Args...>>>
		template<class Function, typename... Args>
		static auto from_code( Function func, Args &&... args ) noexcept {
			using std::swap;
			try {
				func( std::forward<Args>( args )... );
				return expected_t{true};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function, Args...>>>
		template<class Function, typename... Args>
		expected_t( Function func, Args &&... args ) noexcept
		    : expected_t{expected_t::from_code( func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return m_value;
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		bool empty( ) const {
			return !( has_value( ) || has_exception( ) );
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		explicit operator value_type( ) const {
			get( );
		}

		void throw_if_exception( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
		}

		value_type get( ) const {
			throw_if_exception( );
		}

		std::string get_exception_message( ) const {
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { return std::string{e.what( )}; } catch( ... ) {
				return {};
			}
			return {};
		}
	}; // class expected_t<void>

	template<typename ExpectedResult, typename Function, typename... Args>
	auto expected_from_code( Function func, Args &&... args ) noexcept {
		static_assert( std::is_convertible<decltype( func( std::forward<Args>( args )... ) ), ExpectedResult>::value,
		               "Must be able to convert result of func to expected result type" );
		try {
			return expected_t<ExpectedResult>{func( std::forward<Args>( args )... )};
		} catch( ... ) { return expected_t<ExpectedResult>{std::current_exception( )}; }
	}

	template<typename Function, typename... Args>
	decltype( auto ) expected_from_code( Function func, Args &&... args ) noexcept {
		using result_t = std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;
		return expected_from_code<result_t>( std::move( func ), std::forward<Args>( args )... );
	}
} // namespace daw

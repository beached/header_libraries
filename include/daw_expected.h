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
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		daw::optional<value_type> m_value;
		std::exception_ptr m_exception;

	public:
		struct exception_tag {};

		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) = default;
		expected_t( expected_t const & ) = default;
		expected_t &operator=( expected_t const & ) = default;
		expected_t( expected_t && ) noexcept = default;
		expected_t &operator=( expected_t && ) noexcept = default;
		~expected_t( ) = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) == std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t( value_type value ) noexcept
		  : m_value{std::move( value )}
		  , m_exception{} {}

		expected_t &operator=( value_type value ) noexcept {
			m_value = std::move( value );
			m_exception = nullptr;
			return *this;
		}

		expected_t( std::exception_ptr ptr )
		  : m_value{}
		  , m_exception{std::move( ptr )} {}

		expected_t &operator=( std::exception_ptr ptr ) {
			if( m_value ) {
				m_value.reset( );
			}
			m_exception = ptr;
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex )
		  : expected_t{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag )
		  : expected_t{std::current_exception( )} {}

		//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>, std::nullptr_t> = nullptr>
		static expected_t from_code( Function func, Args &&... args ) noexcept {
			try {
				auto tmp = func( std::forward<Args>( args )... );
				return expected_t{std::move( tmp )};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename... Args>
		expected_t( Function func, Args &&... args ) noexcept
		  : expected_t{expected_t::from_code( func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return m_exception;
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

		reference operator*( ) {
			return get( );
		}

		const_reference operator*( ) const {
			return get( );
		}

		pointer operator->( ) {
			return &get( );
		}

		const_pointer operator->( ) const {
			return &get( );
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

	static_assert( daw::is_regular_v<expected_t<int>>, "expected_t isn't regular" );

	template<>
	struct expected_t<void> {
		using value_type = void;

		struct exception_tag {};

	private:
		std::exception_ptr m_exception;
		bool m_value;

		expected_t( bool b ) noexcept
		  : m_exception{}
		  , m_value{b} {}

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( expected_t &&other ) noexcept
		  : m_exception{std::exchange( other.m_exception, nullptr )}
		  , m_value{std::exchange( other.m_value, false )} {}

		expected_t( expected_t const &other ) noexcept
		  : m_exception{}
		  , m_value{other.m_value} {
			if( other.m_exception ) {
				m_exception = other.m_exception;
			}
		}
		expected_t &operator=( expected_t const &rhs ) noexcept {
			if( this != &rhs ) {
				if( rhs.m_exception ) {
					m_exception = rhs.m_exception;
				}
				m_value = rhs.m_value;
			}
			return *this;
		}

		~expected_t( ) = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) == std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////

		expected_t( ) noexcept
		  : expected_t{false} {}

		template<typename T>
		expected_t( T && ) noexcept
		  : expected_t{true} {}

		expected_t &operator=( bool ) noexcept {
			m_value = true;
			m_exception = nullptr;
			return *this;
		}

		expected_t( std::exception_ptr ptr ) noexcept
		  : m_exception{}
		  , m_value{false} {
			daw::exception::daw_throw_on_false( ptr, "Attempt to pass a null exception pointer" );
			m_exception = std::move( ptr );
		}

		expected_t &operator=( std::exception_ptr ptr ) {
			daw::exception::daw_throw_on_false( ptr, "Attempt to pass a null exception pointer" );
			m_value = false;
			m_exception = ptr;
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex ) noexcept
		  : expected_t{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag ) noexcept
		  : expected_t{std::current_exception( )} {}

		//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename Arg, typename... Args>
		static expected_t from_code( Function func, Arg &&arg, Args &&... args ) noexcept {
			try {
				func( std::forward<Arg>( arg ), std::forward<Args>( args )... );
				return expected_t{true};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		template<class Function>
		static expected_t from_code( Function func ) noexcept {
			try {
				func( );
				return expected_t{true};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		//		template<class Function, typename... Args, typename = std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename... Args,
		         typename result = decltype( std::declval<Function>( )( std::declval<Args>( )... ) )>
		expected_t( Function func, Args &&... args ) noexcept
		  : expected_t{expected_t::from_code( func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return m_value;
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return m_exception;
		}

		bool empty( ) const {
			return !( has_value( ) || has_exception( ) );
		}

		explicit operator bool( ) const {
			return !empty( );
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
		static_assert( is_convertible_v<decltype( func( std::forward<Args>( args )... ) ), ExpectedResult>,
		               "Must be able to convert result of func to expected result type" );
		try {
			return expected_t<ExpectedResult>{func, std::forward<Args>( args )...};
		} catch( ... ) { return expected_t<ExpectedResult>{std::current_exception( )}; }
	}

	template<typename Function, typename... Args>
	decltype( auto ) expected_from_code( Function func, Args &&... args ) noexcept {
		using result_t = std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;
		return expected_from_code<result_t>( std::move( func ), std::forward<Args>( args )... );
	}

	template<typename ExpectedType>
	auto expected_from_exception( ) noexcept {
		return expected_t<ExpectedType>{std::current_exception( )};
	}

	template<typename ExpectedType>
	auto expected_from_exception( std::exception_ptr ptr ) noexcept {
		return expected_t<ExpectedType>{ptr};
	}
} // namespace daw

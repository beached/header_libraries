// The MIT License ( MIT )
//
// Copyright ( c ) 2018 Darrell Wright
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

#include <boost/variant.hpp>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_traits.h"

namespace daw {
	struct empty_expected_exception : std::exception {};
	namespace impl {
		struct empty_value_t {};
		constexpr bool operator==( empty_value_t, empty_value_t ) noexcept {
			return true;
		}

		template<typename T, typename THandler, typename EmptyHandler, typename ExceptionHandler>
		struct expected_visitor {
			mutable THandler T_handler;
			mutable EmptyHandler empty_handler;
			mutable ExceptionHandler exception_handler;

			static_assert( daw::is_callable_v<THandler, T const &>, "THandler must accept a single argument of type T" );
			static_assert( daw::is_callable_v<EmptyHandler>, "EmptyHandler must accept no arguments" );
			static_assert( daw::is_callable_v<ExceptionHandler, std::exception_ptr>,
			               "ExceptionHandler must accept a single argument of type std::exception_ptr" );

			decltype( auto ) operator( )( T const &value ) const noexcept {
				return T_handler( value );
			}

			decltype( auto ) operator( )( empty_value_t ) const noexcept {
				return empty_handler( );
			}

			decltype( auto ) operator( )( std::exception_ptr ptr ) const noexcept {
				return exception_handler( ptr );
			}
		};

		template<typename T, typename THandler, typename EmptyHandler, typename ExceptionHandler>
		struct expected_void_visitor {
			mutable THandler T_handler;
			mutable EmptyHandler empty_handler;
			mutable ExceptionHandler exception_handler;

			static_assert( daw::is_callable_v<THandler, T const &>, "THandler must accept a single argument of type T" );
			static_assert( daw::is_callable_v<EmptyHandler>, "EmptyHandler must accept no arguments" );
			static_assert( daw::is_callable_v<ExceptionHandler, std::exception_ptr>,
			               "ExceptionHandler must accept a single argument of type std::exception_ptr" );

			void operator( )( T const &value ) const noexcept {
				T_handler( value );
			}

			void operator( )( empty_value_t ) const noexcept {
				empty_handler( );
			}

			void operator( )( std::exception_ptr ptr ) const noexcept {
				exception_handler( ptr );
			}
		};
	} // namespace impl

	template<typename T, typename THandler, typename EmptyHandler, typename ExceptionHandler>
	impl::expected_visitor<T, THandler, EmptyHandler, ExceptionHandler>
	make_expected_visitor( THandler &&T_handler, EmptyHandler &&empty_handler,
	                       ExceptionHandler &&exception_handler ) noexcept {

		static_assert( daw::is_callable_v<THandler, T const &> || daw::is_callable_v<THandler, T &&>, "THandler must accept a single argument of type T" );
		static_assert( daw::is_callable_v<EmptyHandler>, "EmptyHandler must accept no arguments" );
		static_assert( daw::is_callable_v<ExceptionHandler, std::exception_ptr>,
		               "ExceptionHandler must accept a single argument of type std::exception_ptr" );

		return impl::expected_visitor<T, THandler, EmptyHandler, ExceptionHandler>{
		  std::forward<THandler>( T_handler ), std::forward<EmptyHandler>( empty_handler ),
		  std::forward<ExceptionHandler>( exception_handler )};
	}

	template<typename T, typename THandler, typename EmptyHandler, typename ExceptionHandler>
	impl::expected_void_visitor<T, THandler, EmptyHandler, ExceptionHandler>
	make_expected_void_visitor( THandler &&T_handler, EmptyHandler &&empty_handler,
	                       ExceptionHandler &&exception_handler ) noexcept {

		static_assert( daw::is_callable_v<THandler, T const &> || daw::is_callable_v<THandler, T &&>, "THandler must accept a single argument of type T" );
		static_assert( daw::is_callable_v<EmptyHandler>, "EmptyHandler must accept no arguments" );
		static_assert( daw::is_callable_v<ExceptionHandler, std::exception_ptr>,
		               "ExceptionHandler must accept a single argument of type std::exception_ptr" );

		return impl::expected_void_visitor<T, THandler, EmptyHandler, ExceptionHandler>{
		  std::forward<THandler>( T_handler ), std::forward<EmptyHandler>( empty_handler ),
		  std::forward<ExceptionHandler>( exception_handler )};
	}

	template<class T>
	struct expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		boost::variant<impl::empty_value_t, value_type, std::exception_ptr> m_value;

	public:
		struct exception_tag {};
		expected_t( ) noexcept = default;
		expected_t( expected_t const & ) = default;
		expected_t( expected_t && ) noexcept = default;
		expected_t &operator=( expected_t const & ) = default;
		expected_t &operator=( expected_t && ) = default;
		~expected_t( ) noexcept = default;

		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return lhs.m_value == rhs.m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t( value_type &&value ) noexcept( std::is_nothrow_move_constructible<value_type>::value )
		  : m_value{std::move( value )} {}

		expected_t( value_type const &value ) noexcept( std::is_nothrow_copy_constructible<value_type>::value )
		  : m_value{value} {}

		expected_t &operator=( value_type &&value ) noexcept( std::is_nothrow_move_assignable<value_type>::value ) {
			m_value = std::move( value );
			return *this;
		}

		expected_t &operator=( value_type const &value ) noexcept( std::is_nothrow_copy_assignable<value_type>::value ) {
			m_value = value;
			return *this;
		}

		expected_t( std::exception_ptr ptr ) noexcept
		  : m_value{ptr} {}

		expected_t &operator=( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
			return *this;
		}

		void clear( ) noexcept {
			m_value = impl::empty_value_t{};
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex ) noexcept
		  : m_value{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag ) noexcept
		  : m_value{std::current_exception( )} {}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>, std::nullptr_t> = nullptr>
		static expected_t from_code( Function &&func, Args &&... args ) noexcept {
			try {
				return expected_t{func( std::forward<Args>( args )... )};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		template<typename Visitor>
		decltype(auto) visit( Visitor && visitor ) {
			return boost::apply_visitor( std::forward<Visitor>( visitor ), m_value );
		}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>, std::nullptr_t> = nullptr>
		expected_t( Function &&func, Args &&... args ) noexcept
		  : expected_t{expected_t::from_code( std::forward<Function>( func ), std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return true; },
			                                                                []( ) { return false; },
			                                                                []( std::exception_ptr ) { return false; } ),
			                             m_value );
		}

		bool has_exception( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return false; },
			                                                                []( ) { return false; },
			                                                                []( std::exception_ptr ) { return true; } ),
			                             m_value );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return boost::get<std::exception_ptr>( m_value );
		}

		bool empty( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return false; },
			                                                                []( ) { return true; },
			                                                                []( std::exception_ptr ) { return false; } ),
			                             m_value );
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		explicit operator value_type( ) const {
			return get( );
		}

		void throw_if_exception( ) const {
			boost::apply_visitor(
			  make_expected_visitor<value_type>( []( value_type const & ) {}, []( ) {},
			                                     []( std::exception_ptr ptr ) { std::rethrow_exception( ptr ); } ),
			  m_value );
		}

		reference get( ) {
			throw_if_exception( );
			return boost::get<value_type>( m_value );
		}

		const_reference get( ) const {
			throw_if_exception( );
			return boost::get<value_type>( m_value );
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
			std::string result{};
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
			return result;
		}
	}; // class expected_t

	static_assert( daw::is_regular_v<expected_t<int>>, "expected_t isn't regular" );

	namespace impl {
		struct void_value_t {};
		constexpr bool operator==( void_value_t, void_value_t ) noexcept {
			return true;
		}
	} // namespace impl

	template<>
	struct expected_t<void> {
		using value_type = impl::void_value_t;

		struct exception_tag {};

	private:
		boost::variant<impl::empty_value_t, value_type, std::exception_ptr> m_value;

		expected_t( bool b ) noexcept
		  : m_value{value_type{}} {}

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) noexcept = default;
		expected_t( expected_t const & ) = default;
		expected_t( expected_t && ) noexcept = default;
		expected_t &operator=( expected_t const & ) = default;
		expected_t &operator=( expected_t && ) = default;
		~expected_t( ) noexcept = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return lhs.m_value == rhs.m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////

		template<typename T>
		expected_t( T && ) noexcept
		  : m_value{value_type{}} {}

		expected_t &operator=( bool ) noexcept {
			m_value = value_type{};
			return *this;
		}

		void clear( ) noexcept {
			m_value = impl::empty_value_t{};
		}

		expected_t( std::exception_ptr ptr ) noexcept
		  : m_value{ptr} {}

		expected_t &operator=( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType const &ex ) noexcept
		  : m_value{std::make_exception_ptr( ex )} {}

		expected_t( exception_tag ) noexcept
		  : m_value{std::current_exception( )} {}

		template<class Function, typename... Args>
		static expected_t from_code( Function &&func, Args &&... args ) noexcept {
			static_assert( daw::is_callable_v<Function, Args...>, "Function cannot be called with arguments provided" );
			try {
				func( std::forward<Args>( args )... );
				return expected_t{value_type{}};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		template<typename Visitor>
		decltype(auto) visit( Visitor && visitor ) {
			return boost::apply_visitor( std::forward<Visitor>( visitor ), m_value );
		}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>, std::nullptr_t> = nullptr>
		expected_t( Function &&func, Args &&... args ) noexcept
		  : expected_t{expected_t::from_code( std::forward<Function>( func ), std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return true; },
			                                                                []( ) { return false; },
			                                                                []( std::exception_ptr ) { return false; } ),
			                             m_value );
		}

		bool has_exception( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return false; },
			                                                                []( ) { return false; },
			                                                                []( std::exception_ptr ) { return true; } ),
			                             m_value );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return boost::get<std::exception_ptr>( m_value );
		}

		bool empty( ) const noexcept {
			return boost::apply_visitor( make_expected_visitor<value_type>( []( value_type const & ) { return false; },
			                                                                []( ) { return true; },
			                                                                []( std::exception_ptr ) { return false; } ),
			                             m_value );
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		void throw_if_exception( ) const {
			boost::apply_visitor(
			  make_expected_visitor<value_type>( []( value_type const & ) {}, []( ) {},
			                                     []( std::exception_ptr ptr ) { std::rethrow_exception( ptr ); } ),
			  m_value );
		}

		void get( ) const {
			throw_if_exception( );
		}

		std::string get_exception_message( ) const {
			std::string result{};
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
			return result;
		}
	}; // class expected_t<void>

	template<typename ExpectedResult, typename Function, typename... Args>
	auto expected_from_code( Function &&func, Args &&... args ) noexcept {
		static_assert( is_convertible_v<decltype( func( std::forward<Args>( args )... ) ), ExpectedResult>,
		               "Must be able to convert result of func to expected result type" );
		try {
			return expected_t<ExpectedResult>{std::forward<Function>( func ), std::forward<Args>( args )...};
		} catch( ... ) { return expected_t<ExpectedResult>{std::current_exception( )}; }
	}

	template<typename Function, typename... Args>
	decltype( auto ) expected_from_code( Function &&func, Args &&... args ) noexcept {
		using result_t = std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;
		return expected_from_code<result_t>( std::forward<Function>( func ), std::forward<Args>( args )... );
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

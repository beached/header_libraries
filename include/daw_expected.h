// The MIT License ( MIT )
//
// Copyright ( c ) 2018 Darrell Wright
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

#include <boost/variant.hpp>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	template<class T>
	struct expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		boost::variant<std::exception_ptr, value_type> m_value =
		  std::exception_ptr{nullptr};

	public:
		struct exception_tag {};
		expected_t( ) = default;

		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return lhs.m_value == rhs.m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		explicit expected_t( value_type &&value )
		  : m_value( std::move( value ) ) {}

		explicit expected_t( value_type const &value )
		  : m_value( value ) {}

		expected_t &operator=( value_type &&value ) {
			m_value = std::move( value );
			return *this;
		}

		expected_t &operator=( value_type const &value ) {
			m_value = value;
			return *this;
		}

		expected_t( std::exception_ptr ptr )
		  : m_value( ptr ) {}

		expected_t &operator=( std::exception_ptr ptr ) {
			m_value = ptr;
			return *this;
		}

		void clear( ) {
			m_value = std::exception_ptr{nullptr};
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType &&ex )
		  : m_value(
		      std::make_exception_ptr( std::forward<ExceptionType>( ex ) ) ) {}

		explicit expected_t( exception_tag )
		  : m_value( std::current_exception( ) ) {}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		static expected_t from_code( Function &&func, Args &&... args ) {
			try {
				return expected_t{func( std::forward<Args>( args )... )};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor &&visitor ) {
			return boost::apply_visitor( std::forward<Visitor>( visitor ), m_value );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor &&visitor ) const {
			return boost::apply_visitor( std::forward<Visitor>( visitor ), m_value );
		}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		expected_t( Function &&func, Args &&... args )
		  : expected_t{expected_t::from_code( std::forward<Function>( func ),
		                                      std::forward<Args>( args )... )} {}

		bool has_value( ) const {
			return boost::apply_visitor(
			  daw::make_overload( []( value_type const & ) noexcept { return true; },
			                      []( std::exception_ptr ) noexcept {
				                      return false;
			                      } ),
			  m_value );
		}

		bool has_exception( ) const {
			return boost::apply_visitor(
			  make_overload( []( value_type const & ) noexcept { return false; },
			                 []( std::exception_ptr ptr ) noexcept {
				                 return ( ptr != nullptr );
			                 } ),
			  m_value );
		}

		std::exception_ptr get_exception_ptr( ) {
			return boost::get<std::exception_ptr>( m_value );
		}

		bool empty( ) const {
			return boost::apply_visitor(
			  make_overload( []( value_type const & ) noexcept { return false; },
			                 []( std::exception_ptr ptr ) noexcept {
				                 return ( ptr == nullptr );
			                 } ),
			  m_value );
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		explicit operator value_type( ) const {
			return get( );
		}

		void throw_if_exception( ) const {
			boost::apply_visitor( make_overload( []( value_type const & ) noexcept {},
			                                     []( std::exception_ptr ptr ) {
				                                     if( ptr != nullptr ) {
					                                     std::rethrow_exception( ptr );
				                                     }
			                                     } ),
			                      m_value );
		}

		reference get( ) {
			return boost::apply_visitor(
			  daw::make_overload( []( reference value ) noexcept
			                        ->reference { return value; },
			                      []( std::exception_ptr ptr ) -> reference {
				                      std::rethrow_exception( ptr );
			                      } ),
			  m_value );
		}

		const_reference get( ) const {
			return boost::apply_visitor(
			  daw::make_overload( []( const_reference value ) noexcept
			                        ->const_reference { return value; },
			                      []( std::exception_ptr ptr ) -> const_reference {
				                      std::rethrow_exception( ptr );
			                      } ),
			  m_value );
		}

		reference operator*( ) {
			return get( );
		}

		const_reference operator*( ) const {
			return get( );
		}

		pointer operator->( ) {
			return boost::apply_visitor(
			  daw::make_overload( []( reference value ) noexcept
			                        ->pointer { return std::addressof( value ); },
			                      []( std::exception_ptr ptr ) -> pointer {
				                      std::rethrow_exception( ptr );
			                      } ),
			  m_value );
		}

		const_pointer operator->( ) const {
			return boost::apply_visitor(
			  daw::make_overload(
			    []( const_reference value ) noexcept->const_pointer {
				    return std::addressof( value );
			    },
			    []( std::exception_ptr ptr ) -> const_pointer {
				    std::rethrow_exception( ptr );
			    } ),
			  m_value );
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

	static_assert( daw::is_regular_v<expected_t<int>>,
	               "expected_t isn't regular" );

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
		boost::variant<value_type, std::exception_ptr> m_value =
		  std::exception_ptr{nullptr};

		expected_t( bool b )
		  : m_value( value_type{} ) {}

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		expected_t( ) = default;

		friend bool operator==( expected_t const &lhs, expected_t const &rhs ) {
			return lhs.m_value == rhs.m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		expected_t &operator=( bool ) {
			m_value = value_type{};
			return *this;
		}

		void clear( ) {
			m_value = std::exception_ptr{nullptr};
		}

		explicit expected_t( std::exception_ptr ptr )
		  : m_value( ptr ) {}

		expected_t &operator=( std::exception_ptr ptr ) {
			m_value = ptr;
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType &&ex )
		  : m_value(
		      std::make_exception_ptr( std::forward<ExceptionType>( ex ) ) ) {}

		explicit expected_t( exception_tag )
		  : m_value( std::current_exception( ) ) {}

		template<class Function, typename... Args>
		static expected_t from_code( Function &&func, Args &&... args ) {
			/*
			 static_assert( daw::is_callable_v<Function, Args...>,
			               "Function cannot be called with arguments provided" );
			              */
			try {
				func( std::forward<Args>( args )... );
				return expected_t{true};
			} catch( ... ) { return expected_t{exception_tag{}}; }
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor &&visitor ) {
			return boost::apply_visitor(
			  daw::make_overload( [&]( value_type const & ) mutable noexcept(
			                        noexcept( visitor( ) ) ) { return visitor( ); },
			                      [&]( value_type && ) mutable noexcept(
			                        noexcept( visitor( ) ) ) { return visitor( ); },
			                      [&]( std::exception_ptr ptr ) mutable noexcept(
			                        noexcept( visitor( ptr ) ) ) {
				                      return visitor( ptr );
			                      } ),
			  m_value );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor &&visitor ) const {
			return boost::apply_visitor(
			  daw::make_overload( [&]( value_type const & ) mutable noexcept(
			                        noexcept( visitor( ) ) ) { return visitor( ); },
			                      [&]( std::exception_ptr ptr ) mutable noexcept(
			                        noexcept( visitor( ptr ) ) ) {
				                      return visitor( ptr );
			                      } ),
			  m_value );
		}

		template<class Function, typename... Args,
		         std::enable_if_t<daw::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		expected_t( Function &&func, Args &&... args )
		  : expected_t{expected_t::from_code( std::forward<Function>( func ),
		                                      std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return boost::apply_visitor(
			  make_overload( []( value_type const & ) noexcept { return true; },
			                 []( std::exception_ptr ) noexcept { return false; } ),
			  m_value );
		}

		bool has_exception( ) const noexcept {
			return boost::apply_visitor(
			  make_overload( []( value_type const & ) noexcept { return false; },
			                 []( std::exception_ptr ptr ) noexcept {
				                 return ( ptr != nullptr );
			                 } ),
			  m_value );
		}

		std::exception_ptr get_exception_ptr( ) {
			return boost::get<std::exception_ptr>( m_value );
		}

		bool empty( ) const noexcept {
			return boost::apply_visitor(
			  make_overload( []( value_type const & ) noexcept { return false; },
			                 []( std::exception_ptr ptr ) noexcept {
				                 return ( ptr == nullptr );
			                 } ),
			  m_value );
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		void throw_if_exception( ) const {
			boost::apply_visitor( make_overload( []( value_type const & ) noexcept {},
			                                     []( std::exception_ptr ptr ) {
				                                     if( ptr != nullptr ) {
					                                     std::rethrow_exception( ptr );
				                                     }
			                                     } ),
			                      m_value );
		}

		void get( ) const {
			throw_if_exception( );
		}

		std::string get_exception_message( ) const noexcept {
			std::string result{};
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
			return result;
		}
	}; // class expected_t<void>

	template<typename ExpectedResult, typename Function, typename... Args>
	auto expected_from_code( Function &&func, Args &&... args ) {
		static_assert(
		  is_convertible_v<decltype( func( std::forward<Args>( args )... ) ),
		                   ExpectedResult>,
		  "Must be able to convert result of func to expected result type" );
		try {
			return expected_t<ExpectedResult>( std::forward<Function>( func ),
			                                   std::forward<Args>( args )... );
		} catch( ... ) {
			return expected_t<ExpectedResult>( std::current_exception( ) );
		}
	}

	template<typename Function, typename... Args>
	decltype( auto ) expected_from_code( Function &&func, Args &&... args ) {
		using result_t =
		  std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;
		return expected_from_code<result_t>( std::forward<Function>( func ),
		                                     std::forward<Args>( args )... );
	}

	template<typename ExpectedType>
	auto expected_from_exception( ) {
		return expected_t<ExpectedType>( std::current_exception( ) );
	}

	template<typename ExpectedType>
	auto expected_from_exception( std::exception_ptr ptr ) {
		return expected_t<ExpectedType>( ptr );
	}
} // namespace daw

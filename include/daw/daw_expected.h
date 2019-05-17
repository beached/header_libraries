// The MIT License ( MIT )
//
// Copyright ( c ) 2018-2019 Darrell Wright
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

#include <cassert>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <variant>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_overload.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "daw_visit.h"

namespace daw {
	template<class T>
	struct expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		struct empty_value_t {};
		std::variant<empty_value_t, std::exception_ptr, value_type> m_value{};

	public:
		struct exception_tag {};
		constexpr expected_t( ) = default;

		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		friend constexpr bool operator==( expected_t const &lhs, expected_t const &rhs ) noexcept {
			return lhs.m_value == rhs.m_value;
		}

		friend constexpr bool operator!=( expected_t const &lhs, expected_t const &rhs ) noexcept {
			return lhs.m_value != rhs.m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		explicit constexpr expected_t( value_type &&value )
		  : m_value( daw::move( value ) ) {}

		explicit constexpr expected_t( value_type const &value )
		  : m_value( value ) {}

		constexpr expected_t &operator=( value_type &&value ) {
			m_value = daw::move( value );
			return *this;
		}

		constexpr expected_t &operator=( value_type const &value ) {
			m_value = value;
			return *this;
		}

		expected_t( std::exception_ptr ptr ) noexcept
		  : m_value( ptr ) {}

		expected_t &operator=( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
			return *this;
		}

		constexpr void clear( ) noexcept {
			m_value = empty_value_t{};
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType &&ex ) noexcept
		  : m_value(
		      std::make_exception_ptr( std::forward<ExceptionType>( ex ) ) ) {}

		explicit expected_t( exception_tag ) noexcept
		  : m_value( std::current_exception( ) ) {}

	private:
		template<class Function, typename... Args,
		         std::enable_if_t<
		           traits::is_callable_convertible_v<value_type, Function, Args...>,
		           std::nullptr_t> = nullptr>
		static std::variant<std::exception_ptr, value_type>
		variant_from_code( Function &&func, Args &&... args ) {
			try {
				return func( std::forward<Args>( args )... );
			} catch( ... ) { return std::current_exception( ); }
		}

	public:
		template<class Function, typename... Args,
		         std::enable_if_t<
		           traits::is_callable_convertible_v<value_type, Function, Args...>,
		           std::nullptr_t> = nullptr>
		explicit expected_t( Function &&func, Args &&... args )
		  : m_value( variant_from_code( std::forward<Function>( func ),
		                                std::forward<Args>( args )... ) ) {}

		template<class Function, typename... Args,
		         std::enable_if_t<
		           traits::is_callable_convertible_v<value_type, Function, Args...>,
		           std::nullptr_t> = nullptr>
		static expected_t from_code( Function &&func, Args &&... args ) {
			auto result = expected_t( );
			result.m_value = variant_from_code( std::forward<Function>( func ),
			                                    std::forward<Args>( args )... );
			return result;
		}

		void set_exception( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
		}

		void set_exception( ) noexcept {
			set_exception( std::current_exception( ) );
		}

		template<typename... Visitors>
		constexpr decltype( auto ) visit( Visitors &&... visitors ) {
			static_assert(
			  daw::is_visitable_v<value_type &, Visitors...>,
			  "Visitor must be callable with the variants expected value_type &" );
			static_assert( daw::is_visitable_v<std::exception_ptr, Visitors...>,
			               "Visitor must be callable with std::exception_ptr" );

			return daw::visit_nt( m_value, std::forward<Visitors>( visitors )... );
		}

		template<typename... Visitors>
		constexpr decltype( auto ) visit( Visitors &&... visitors ) const {
			static_assert(
			  daw::is_visitable_v<value_type const &, Visitors...>,
			  "Visitor must be callable with the variants expected value_type &" );
			static_assert( daw::is_visitable_v<std::exception_ptr, Visitors...>,
			               "Visitor must be callable with std::exception_ptr" );

			return daw::visit_nt( m_value, std::forward<Visitors>( visitors )... );
		}

		bool has_value( ) const {
			return m_value.index( ) == 2U;
		}

		bool has_exception( ) const {
			return m_value.index( ) == 1U;
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return std::get<1U>( m_value );
		}

		constexpr bool empty( ) const noexcept {
			return m_value.index( ) == 0U;
		}

		constexpr explicit operator bool( ) const noexcept {
			return !empty( );
		}

		constexpr explicit operator value_type( ) const {
			return get( );
		}

		void throw_if_exception( ) const {
			if( m_value.index( ) != 0 ) {
				return;
			}
			std::exception_ptr ptr = std::get<0>( m_value );
			if( ptr == nullptr ) {
				return;
			}
			std::rethrow_exception( ptr );
		}

		reference get( ) {
			throw_if_exception( );
			return std::get<2U>( m_value );
		}

		const_reference get( ) const {
			throw_if_exception( );
			return std::get<2U>( m_value );
		}

		reference operator*( ) {
			return get( );
		}

		const_reference operator*( ) const {
			return get( );
		}

		pointer operator->( ) {
			throw_if_exception( );
			return &std::get<2U>( m_value );
		}

		const_pointer operator->( ) const {
			throw_if_exception( );
			return &std::get<2U>( m_value );
		}

		std::string get_exception_message( ) const noexcept {
			auto result = std::string( );
			try {
				throw_if_exception( );
			} catch( std::system_error const &e ) {
				result = e.code( ).message( ) + ": " + e.what( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
			return result;
		}
	}; // namespace daw

	static_assert( traits::is_regular<expected_t<int>> );

	namespace impl {
		struct void_value_t {
			constexpr void_value_t( ) noexcept = default;
		};
		constexpr bool operator==( void_value_t, void_value_t ) noexcept {
			return true;
		}
	} // namespace impl

	template<>
	struct expected_t<void> {
		using value_type = impl::void_value_t;

		struct exception_tag {};

	private:
		std::variant<std::exception_ptr, value_type> m_value =
		  std::exception_ptr{nullptr};

		expected_t( bool )
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
			m_value = daw::construct_a<value_type>{}( );
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

	private:
		template<class Function, typename... Args>
		static std::variant<std::exception_ptr, value_type>
		variant_from_code( Function &&func, Args &&... args ) {
			try {
				func( std::forward<Args>( args )... );
				return impl::void_value_t( );
			} catch( ... ) { return std::current_exception( ); }
		}

	public:
		template<class Function, typename... Args,
		         std::enable_if_t<traits::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		explicit expected_t( Function &&func, Args &&... args )
		  : m_value( variant_from_code( std::forward<Function>( func ),
		                                std::forward<Args>( args )... ) ) {}

		template<class Function, typename... Args,
		         std::enable_if_t<traits::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		static expected_t from_code( Function &&func, Args &&... args ) {
			auto result = expected_t( );
			result.m_value = variant_from_code( std::forward<Function>( func ),
			                                    std::forward<Args>( args )... );
			return result;
		}

		void set_exception( std::exception_ptr ptr ) {
			m_value = ptr;
		}

		void set_exception( ) {
			set_exception( std::current_exception( ) );
		}

		template<typename... Visitors>
		decltype( auto ) visit( Visitors &&... visitors ) {
			auto visitor = daw::overload( std::forward<Visitors>( visitors )... );
			static_assert( traits::is_callable_v<decltype( visitor )>,
			               "Visitor must be callable without arguments" );
			static_assert(
			  traits::is_callable_v<decltype( visitor ), std::exception_ptr>,
			  "Visitor must be callable with std::exception_ptr" );

			return daw::visit_nt(
			  m_value,
			  [&]( value_type const & ) mutable noexcept( noexcept( visitor( ) ) ) {
				  return visitor( );
			  },
			  [&]( value_type && ) mutable noexcept( noexcept( visitor( ) ) ) {
				  return visitor( );
			  },
			  [&]( std::exception_ptr ptr ) mutable noexcept(
			    noexcept( visitor( ptr ) ) ) { return visitor( ptr ); } );
		}

		template<typename... Visitors>
		decltype( auto ) visit( Visitors &&... visitors ) const {
			auto visitor = daw::overload( std::forward<Visitors>( visitors )... );
			static_assert( traits::is_callable_v<decltype( visitor )>,
			               "Visitor must be callable without arguments" );
			static_assert(
			  traits::is_callable_v<decltype( visitor ), std::exception_ptr>,
			  "Visitor must be callable with std::exception_ptr" );

			return daw::visit_nt(
			  m_value,
			  [&]( value_type const & ) mutable noexcept( noexcept( visitor( ) ) ) {
				  return visitor( );
			  },
			  [&]( std::exception_ptr ptr ) mutable noexcept(
			    noexcept( visitor( ptr ) ) ) { return visitor( ptr ); } );
		}

		bool has_value( ) const noexcept {
			return m_value.index( ) == 1U;
		}

		bool has_exception( ) const noexcept {
			return m_value.index( ) == 0U and std::get<0U>( m_value ) != nullptr;
		}

		std::exception_ptr get_exception_ptr( ) {
			return std::get<0U>( m_value );
		}

		bool empty( ) const noexcept {
			return m_value.index( ) == 0U and std::get<0U>( m_value ) == nullptr;
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		void throw_if_exception( ) const {
			if( m_value.index( ) != 0 ) {
				return;
			}
			std::exception_ptr ptr = std::get<0>( m_value );
			if( ptr == nullptr ) {
				return;
			}
			std::rethrow_exception( ptr );
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

	template<typename Result, typename Function, typename... Args>
	expected_t<Result> expected_from_code( Function &&func, Args &&... args ) {
		static_assert(
		  traits::is_callable_convertible_v<Result, Function, Args...>,
		  "Must be able to convert result of func to expected result type" );

		auto result = expected_t<Result>( );
		result.from_code( std::forward<Function>( func ),
		                  std::forward<Args>( args )... );
		return result;
	}

	template<typename Function, typename... Args>
	auto expected_from_code( Function &&func, Args &&... args ) {
		using result_t =
		  std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;

		return expected_t<result_t>::from_code( std::forward<Function>( func ),
		                                        std::forward<Args>( args )... );
	}

	template<typename Result>
	expected_t<Result> expected_from_exception( ) {
		return expected_t<Result>( std::current_exception( ) );
	}

	template<typename Result>
	expected_t<Result> expected_from_exception( std::exception_ptr ptr ) {
		return expected_t<Result>( ptr );
	}
} // namespace daw

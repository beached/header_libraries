// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_move.h"
#include "impl/daw_traits_impl.h"
#include "traits/daw_traits_concepts.h"

#include <daw/stdinc/move_fwd_exch.h>
#include <exception>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

namespace daw::expected_details {
	enum expected_value_types : bool { Empty, Void };
	template<expected_value_types>
	struct ExpectedTag {
		ExpectedTag( ) = default;
		constexpr bool operator==( ExpectedTag const & ) const noexcept {
			return true;
		}

		constexpr bool operator!=( ExpectedTag const & ) const noexcept {
			return false;
		}
	};
} // namespace daw::expected_details

namespace daw {
	template<class T>
	struct expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		using variant_type =
		  std::variant<expected_details::ExpectedTag<expected_details::Empty>,
		               std::exception_ptr,
		               value_type>;
		variant_type m_value{ };

	public:
		struct exception_tag {};

		expected_t( ) = default;

		/// Summary: No value, aka null
		bool operator==( expected_t const &rhs ) const noexcept {
			return m_value == rhs.m_value;
		}

		bool operator!=( expected_t const &rhs ) const noexcept {
			return m_value != rhs.m_value;
		}

		/// Summary: With value
		explicit expected_t( value_type &&value )
		  : m_value( std::move( value ) ) {}

		explicit expected_t( value_type const &value )
		  : m_value( value ) {}

		void set_value( value_type const &value ) {
			m_value = value;
		}
		void set_value( value_type &&value ) noexcept(
		  std::is_nothrow_move_assignable_v<value_type> ) {
			m_value = std::move( value );
		}

		expected_t &operator=( value_type &&value ) {
			set_value( std::move( value ) );
			return *this;
		}

		expected_t &operator=( value_type const &value ) {
			set_value( value );
			return *this;
		}

		expected_t( std::exception_ptr ptr ) noexcept
		  : m_value( ptr ) {}

		expected_t &operator=( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
			return *this;
		}

		void clear( ) noexcept {
			m_value = variant_type( );
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType &&ex ) noexcept
		  : m_value( std::make_exception_ptr( DAW_FWD2( ExceptionType, ex ) ) ) {}

		explicit expected_t( exception_tag ) noexcept
		  : m_value( std::current_exception( ) ) {}

	private:
		template<
		  typename Function,
		  typename... Args,
		  std::enable_if_t<std::is_invocable_r_v<value_type, Function, Args...>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] inline static variant_type
		variant_from_code( Function &&func, Args &&...args ) {
#if defined( DAW_USE_EXCEPTIONS )
			try {
				return DAW_FWD( func )( DAW_FWD( args )... );
			} catch( ... ) { return std::current_exception( ); }
#else
			return DAW_FWD( func )( DAW_FWD( args )... );
#endif
		}

	public:
		template<
		  class Function,
		  typename... Args,
		  std::enable_if_t<std::is_invocable_r_v<value_type, Function, Args...>,
		                   std::nullptr_t> = nullptr>
		explicit expected_t( Function &&func, Args &&...args )
		  : m_value( variant_from_code( DAW_FWD( func ), DAW_FWD( args )... ) ) {}

		template<
		  class Function,
		  typename... Args,
		  std::enable_if_t<std::is_invocable_r_v<value_type, Function, Args...>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] static expected_t from_code( Function &&func,
		                                           Args &&...args ) {
			auto result = expected_t( );
			result.m_value = variant_from_code( DAW_FWD( func ), DAW_FWD( args )... );
			return result;
		}

		void set_exception( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
		}

		void set_exception( ) noexcept {
			set_exception( std::current_exception( ) );
		}

		[[nodiscard]] bool has_value( ) const noexcept {
			return std::holds_alternative<value_type>( m_value );
		}

		[[nodiscard]] bool has_exception( ) const noexcept {
			return std::holds_alternative<std::exception_ptr>( m_value );
		}

		[[nodiscard]] std::exception_ptr get_exception_ptr( ) const noexcept {
			return *std::get_if<std::exception_ptr>( &m_value );
		}

		[[nodiscard]] bool empty( ) const noexcept {
			return std::holds_alternative<
			  expected_details::ExpectedTag<expected_details::Empty>>( m_value );
		}

		explicit operator bool( ) const noexcept {
			return not empty( );
		}

		void throw_if_exception( ) const {
			if( not has_exception( ) ) {
				return;
			}
			std::rethrow_exception( *std::get_if<std::exception_ptr>( &m_value ) );
		}

		[[nodiscard]] reference get( ) & {
			if( empty( ) ) {
				std::terminate( );
			}
			throw_if_exception( );
			return *std::get_if<value_type>( &m_value );
		}

		[[nodiscard]] const_reference get( ) const & {
			if( empty( ) ) {
				std::terminate( );
			}
			throw_if_exception( );
			return *std::get_if<value_type>( &m_value );
		}

		[[nodiscard]] value_type get( ) && {
			if( empty( ) ) {
				std::terminate( );
			}
			throw_if_exception( );
			return std::move( *std::get_if<value_type>( &m_value ) );
		}

		[[nodiscard]] const_reference get( ) const && {
			if( empty( ) ) {
				std::terminate( );
			}
			throw_if_exception( );
			return *std::get_if<value_type>( &m_value );
		}

		[[nodiscard]] reference operator*( ) {
			return get( );
		}

		[[nodiscard]] const_reference operator*( ) const {
			return get( );
		}

		[[nodiscard]] pointer operator->( ) {
			return &( get( ) );
		}

		[[nodiscard]] const_pointer operator->( ) const {
			return &( get( ) );
		}

		[[nodiscard]] std::string get_exception_message( ) const noexcept {
			auto result = std::string( );
#if defined( DAW_USE_EXCEPTIONS )
			try {
				throw_if_exception( );
			} catch( std::system_error const &e ) {
				result = e.code( ).message( ) + ": " + e.what( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
#endif
			return result;
		}
	};

	static_assert( daw::traits::is_regular<expected_t<int>> );

	template<>
	struct expected_t<void> {
		using value_type = expected_details::ExpectedTag<expected_details::Void>;

		struct exception_tag {};

	private:
		using variant_type =
		  std::variant<expected_details::ExpectedTag<expected_details::Empty>,
		               std::exception_ptr,
		               value_type>;
		variant_type m_value{ };

		expected_t( bool ) noexcept
		  : m_value( value_type{ } ) {}

	public:
		/// Summary: No value, aka null
		expected_t( ) = default;

		friend bool operator==( expected_t const &lhs,
		                        expected_t const &rhs ) noexcept {
			return lhs.m_value == rhs.m_value;
		}

		friend bool operator!=( expected_t const &lhs,
		                        expected_t const &rhs ) noexcept {
			return lhs.m_value != rhs.m_value;
		}

		void set_value( ) noexcept {
			m_value = value_type( );
		}

		template<typename T>
		void set_value( T && ) noexcept {
			m_value = value_type( );
		}

		/// Summary: With value
		expected_t &operator=( bool ) noexcept {
			set_value( );
			return *this;
		}

		void clear( ) noexcept {
			m_value = variant_type( );
		}

		explicit expected_t( std::exception_ptr ptr ) noexcept
		  : m_value( ptr ) {}

		expected_t &operator=( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
			return *this;
		}

		template<typename ExceptionType>
		expected_t( exception_tag, ExceptionType &&ex ) noexcept
		  : m_value( std::make_exception_ptr( DAW_FWD2( ExceptionType, ex ) ) ) {}

		explicit expected_t( exception_tag ) noexcept
		  : m_value( std::current_exception( ) ) {}

	private:
		template<class Function, typename... Args>
		[[nodiscard]] static variant_type
		variant_from_code( Function &&func, Args &&...args ) noexcept {
#if defined( DAW_USE_EXCEPTIONS )
			try {
				(void)func( DAW_FWD( args )... );
				return expected_details::ExpectedTag<expected_details::Void>( );
			} catch( ... ) { return std::current_exception( ); }
#else
			(void)func( DAW_FWD( args )... );
			return expected_details::ExpectedTag<expected_details::Void>( );
#endif
		}

	public:
		template<class Function,
		         typename... Args,
		         std::enable_if_t<std::is_invocable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		explicit expected_t( Function &&func, Args &&...args ) noexcept
		  : m_value( variant_from_code( DAW_FWD( func ), DAW_FWD( args )... ) ) {}

		template<class Function,
		         typename... Args,
		         std::enable_if_t<std::is_invocable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] static expected_t from_code( Function &&func,
		                                           Args &&...args ) noexcept {
			auto result = expected_t( );
			result.m_value = variant_from_code( DAW_FWD( func ), DAW_FWD( args )... );
			return result;
		}

		void set_exception( std::exception_ptr ptr ) noexcept {
			m_value = ptr;
		}

		void set_exception( ) noexcept {
			set_exception( std::current_exception( ) );
		}

		[[nodiscard]] bool has_value( ) const noexcept {
			return std::holds_alternative<value_type>( m_value );
		}

		[[nodiscard]] bool has_exception( ) const noexcept {
			return std::holds_alternative<std::exception_ptr>( m_value );
		}

		[[nodiscard]] std::exception_ptr get_exception_ptr( ) const noexcept {
			return *std::get_if<std::exception_ptr>( &m_value );
		}

		[[nodiscard]] bool empty( ) const noexcept {
			return std::holds_alternative<
			  expected_details::ExpectedTag<expected_details::Empty>>( m_value );
		}

		explicit operator bool( ) const noexcept {
			return not empty( );
		}

		void throw_if_exception( ) const {
			if( not has_exception( ) ) {
				return;
			}
			std::rethrow_exception( *std::get_if<std::exception_ptr>( &m_value ) );
		}

		void get( ) const {
			if( empty( ) ) {
				std::terminate( );
			}
			throw_if_exception( );
		}

		[[nodiscard]] std::string get_exception_message( ) const noexcept {
			std::string result{ };
#if defined( DAW_USE_EXCEPTIONS )
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) { result = e.what( ); } catch( ... ) {
			}
#endif
			return result;
		}
	}; // class expected_t<void>

	template<typename Result, typename Function, typename... Args>
	[[nodiscard]] expected_t<Result> expected_from_code( Function &&func,
	                                                     Args &&...args ) {
		static_assert(
		  daw::traits::is_callable_convertible_v<Result, Function, Args...>,
		  "Must be able to convert result of func to expected result type" );

		auto result = expected_t<Result>( );
		result.from_code( DAW_FWD( func ), DAW_FWD( args )... );
		return result;
	}

	template<typename Function, typename... Args>
	[[nodiscard]] auto expected_from_code( Function &&func, Args &&...args ) {
		using result_t = std::decay_t<decltype( func( DAW_FWD( args )... ) )>;

		return expected_t<result_t>::from_code( DAW_FWD( func ),
		                                        DAW_FWD( args )... );
	}

	template<typename Result>
	[[nodiscard]] expected_t<Result> expected_from_exception( ) {
		return expected_t<Result>( std::current_exception( ) );
	}

	template<typename Result>
	[[nodiscard]] expected_t<Result>
	expected_from_exception( std::exception_ptr ptr ) {
		return expected_t<Result>( ptr );
	}
} // namespace daw

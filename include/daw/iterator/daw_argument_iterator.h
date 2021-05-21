// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../daw_exception.h"
#include "../daw_traits.h"

#include <ciso646>
#include <stdexcept>
#include <string_view>

namespace daw {
	class arg_iterator_t {
		int m_argc;
		int m_pos = 0;
		char const *const *m_argv;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using pointer = std::string_view *;
		using difference_type = std::ptrdiff_t;
		using value_type = std::string_view;
		using reference = std::string_view;

		constexpr arg_iterator_t( ) noexcept
		  : m_argc( 0 )
		  , m_argv( nullptr ) {}

		constexpr arg_iterator_t( int argc, char const *const *argv ) noexcept
		  : m_argc( argc )
		  , m_argv( argv ) {}

		[[nodiscard]] constexpr std::string_view operator*( ) const noexcept {
			return std::string_view( m_argv[m_pos] );
		}

		constexpr arg_iterator_t &operator++( ) noexcept {
			++m_pos;
			return *this;
		}

		constexpr arg_iterator_t &operator--( ) noexcept {
			--m_pos;
			return *this;
		}

		constexpr arg_iterator_t operator++( int ) noexcept {
			auto result = arg_iterator_t( *this );
			++m_pos;
			return result;
		}

		constexpr arg_iterator_t operator--( int ) noexcept {
			auto result = arg_iterator_t( *this );
			--m_pos;
			return result;
		}

		constexpr arg_iterator_t operator+( difference_type n ) const noexcept {
			auto result = arg_iterator_t( *this );
			result.m_pos += static_cast<int>( n );
			return result;
		}

		constexpr arg_iterator_t &operator+=( difference_type n ) noexcept {
			m_pos += static_cast<int>( n );
			return *this;
		}

		constexpr arg_iterator_t operator-( difference_type n ) const noexcept {
			auto result = arg_iterator_t( *this );
			result.m_pos -= static_cast<int>( n );
			return result;
		}

		constexpr arg_iterator_t &operator-=( difference_type n ) noexcept {
			m_pos -= static_cast<int>( n );
			return *this;
		}

		constexpr difference_type
		operator-( arg_iterator_t const &rhs ) const noexcept {
			return m_pos - rhs.m_pos;
		}

		constexpr difference_type
		operator+( arg_iterator_t const &rhs ) const noexcept {
			return m_pos + rhs.m_pos;
		}

		[[nodiscard]] constexpr std::string_view
		operator[]( difference_type n ) const noexcept {
			return std::string_view( m_argv[m_pos + n] );
		}

		[[nodiscard]] constexpr std::string_view at( difference_type n ) const {
			auto const pos = m_pos + n;

			daw::exception::precondition_check<std::out_of_range>(
			  0 < pos and pos < m_argc, "Attempt to access invalid argument" );

			return std::string_view( m_argv[pos] );
		}

		[[nodiscard]] constexpr bool at_end( ) const noexcept {
			return m_argv == nullptr or m_pos >= m_argc;
		}

		constexpr bool operator==( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_pos == rhs.m_pos;
			}
			return at_end( ) and rhs.at_end( );
		}

		constexpr bool operator!=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_pos != rhs.m_pos;
			}
			return not( at_end( ) and rhs.at_end( ) );
		}

		constexpr bool operator<( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc < rhs.m_argc;
			}
			return not at_end( ) and rhs.at_end( );
		}

		constexpr bool operator<=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc <= rhs.m_argc;
			}
			return rhs.at_end( );
		}

		constexpr bool operator>( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc > rhs.m_argc;
			}
			return at_end( );
		}

		constexpr bool operator>=( arg_iterator_t const &rhs ) const noexcept {
			if( m_argv == rhs.m_argv ) {
				return m_argc <= rhs.m_argc;
			}
			return at_end( );
		}

		[[nodiscard]] constexpr size_t size( ) const noexcept {
			return static_cast<size_t>( m_argc );
		}

		[[nodiscard]] constexpr difference_type position( ) const noexcept {
			return static_cast<difference_type>( m_pos );
		}

		[[nodiscard]] constexpr arg_iterator_t begin( ) const noexcept {
			return *this;
		}

		[[nodiscard]] static constexpr arg_iterator_t end( ) noexcept {
			return arg_iterator_t( );
		}

		[[nodiscard]] constexpr operator bool( ) const noexcept {
			return 0 <= m_pos and m_pos < m_argc;
		}
	};
} // namespace daw

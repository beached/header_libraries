// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>
#include <iterator>
#include <limits>

namespace daw {
	template<typename CharT = char>
	struct repeat_n_char_iterator {
		using value_type = CharT;
		using difference_type = ptrdiff_t;
		using pointer = value_type const *;
		using const_pointer = value_type const *;
		using reference = value_type const &;
		using const_reference = value_type const &;
		using iterator_category = std::random_access_iterator_tag;

		value_type m_value = 0;
		ptrdiff_t m_position = 0;

		constexpr repeat_n_char_iterator( ) noexcept = default;

		constexpr repeat_n_char_iterator( size_t count, value_type value ) noexcept
		  : m_value( value )
		  , m_position( static_cast<ptrdiff_t>( count ) ) {}

		constexpr reference operator*( ) const noexcept {
			return m_value;
		}

		constexpr pointer operator->( ) const noexcept {
			return &m_value;
		}

		constexpr repeat_n_char_iterator &operator++( ) noexcept {
			--m_position;
			return *this;
		}

		constexpr repeat_n_char_iterator operator++( int ) noexcept {
			auto tmp = *this;
			--m_position;
			return tmp;
		}

		constexpr repeat_n_char_iterator &operator--( ) noexcept {
			++m_position;
			return *this;
		}

		constexpr repeat_n_char_iterator operator--( int ) noexcept {
			auto tmp = *this;
			++m_position;
			return tmp;
		}

		constexpr reference operator[]( size_t ) const noexcept {
			return m_value;
		}

		constexpr bool equal( repeat_n_char_iterator const &rhs ) const noexcept {
			return m_position == rhs.m_position;
		}

		constexpr repeat_n_char_iterator const &operator+=( ptrdiff_t p ) noexcept {
			m_position += p;
			return *this;
		}

		constexpr repeat_n_char_iterator const &operator-=( ptrdiff_t p ) noexcept {
			m_position -= p;
			return *this;
		}

		constexpr repeat_n_char_iterator operator+( ptrdiff_t p ) const noexcept {
			auto tmp = *this;
			tmp += p;
			return tmp;
		}

		constexpr repeat_n_char_iterator operator-( ptrdiff_t p ) const noexcept {
			auto tmp = *this;
			tmp -= p;
			return tmp;
		}

		constexpr ptrdiff_t
		operator-( repeat_n_char_iterator const &rhs ) const noexcept {
			return rhs.m_position - m_position;
		}
	};
	template<typename CharT>
	repeat_n_char_iterator( size_t, CharT ) -> repeat_n_char_iterator<CharT>;

	template<typename T = char>
	constexpr auto repeat_n_char_end( ) noexcept {
		return repeat_n_char_iterator<T>( );
	}

	template<typename CharT>
	constexpr bool
	operator==( repeat_n_char_iterator<CharT> const &lhs,
	            repeat_n_char_iterator<CharT> const &rhs ) noexcept {

		return lhs.equal( rhs );
	}

	template<typename CharT>
	constexpr bool
	operator!=( repeat_n_char_iterator<CharT> const &lhs,
	            repeat_n_char_iterator<CharT> const &rhs ) noexcept {

		return !lhs.equal( rhs );
	}
} // namespace daw

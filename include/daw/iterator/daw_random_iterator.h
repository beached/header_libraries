// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"

#include <cstddef>
#include <iterator>

namespace daw {
	template<typename T>
	struct RandomIterator {
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;

	private:
		pointer m_pointer = nullptr;

	public:
		explicit RandomIterator( ) = default;

		explicit constexpr RandomIterator( T *ptr ) noexcept
		  : m_pointer{ ptr } {}

		constexpr RandomIterator &operator=( T *rhs ) noexcept {
			m_pointer = rhs;
			return *this;
		}

		[[nodiscard]] constexpr pointer ptr( ) noexcept {
			return m_pointer;
		}

		[[nodiscard]] constexpr pointer ptr( ) const noexcept {
			return m_pointer;
		}

		constexpr RandomIterator &operator+=( std::ptrdiff_t n ) {
			m_pointer += n;
			return *this;
		}

		constexpr RandomIterator &operator-=( std::ptrdiff_t n ) {
			m_pointer -= n;
			return *this;
		}

		[[nodiscard]] constexpr reference operator*( ) const {
			return *m_pointer;
		}

		[[nodiscard]] constexpr reference operator[]( std::size_t idx ) const {
			return *std::next( m_pointer, static_cast<std::ptrdiff_t>( idx ) );
		}

		[[nodiscard]] constexpr pointer operator->( ) const noexcept {
			return m_pointer;
		}

		constexpr RandomIterator &operator++( ) noexcept {
			++m_pointer;
			return *this;
		}

		constexpr RandomIterator operator++( int ) noexcept {
			auto result = RandomIterator{ *this };
			++m_pointer;
			return result;
		}

		constexpr RandomIterator &operator--( ) noexcept {
			--m_pointer;
			return *this;
		}

		constexpr RandomIterator operator--( int ) noexcept {
			auto result = RandomIterator{ *this };
			--m_pointer;
			return result;
		}

		friend constexpr RandomIterator operator+( RandomIterator lhs,
		                                           std::ptrdiff_t n ) noexcept {
			lhs += n;
			return lhs;
		}

		friend constexpr RandomIterator operator+( std::ptrdiff_t n,
		                                           RandomIterator rhs ) noexcept {
			rhs += n;
			return rhs;
		}

		friend constexpr RandomIterator operator-( RandomIterator lhs,
		                                           std::ptrdiff_t n ) noexcept {
			lhs += n;
			return lhs;
		}

		friend constexpr RandomIterator operator-( std::ptrdiff_t n,
		                                           RandomIterator rhs ) noexcept {
			rhs -= n;
			return rhs;
		}

		constexpr friend bool operator==( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer == rhs.m_pointer;
		}

		constexpr friend bool operator!=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer != rhs.m_pointer;
		}

		constexpr friend bool operator<( RandomIterator const &lhs,
		                                 RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer < rhs.m_pointer;
		}

		constexpr friend bool operator>( RandomIterator const &lhs,
		                                 RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer > rhs.m_pointer;
		}

		constexpr friend bool operator<=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer <= rhs.m_pointer;
		}

		constexpr friend bool operator>=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer >= rhs.m_pointer;
		}

		constexpr friend std::ptrdiff_t
		operator-( RandomIterator const &lhs, RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer - rhs.m_pointer;
		}
	}; // RandomIterator

	template<typename T>
	[[nodiscard]] constexpr RandomIterator<T>
	make_random_iterator( T *ptr ) noexcept {
		return RandomIterator<T>{ ptr };
	}

	template<typename T>
	[[nodiscard]] constexpr RandomIterator<T const>
	make_random_iterator( T const *ptr ) noexcept {
		return RandomIterator<T const>{ ptr };
	}

	template<typename T>
	[[nodiscard]] constexpr RandomIterator<T const>
	make_const_random_iterator( T *const ptr ) noexcept {
		return RandomIterator<T const>{ ptr };
	}
} // namespace daw

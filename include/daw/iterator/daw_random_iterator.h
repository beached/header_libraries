// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"

#include <iterator>

namespace daw {
	template<typename T>
	struct RandomIterator {
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		pointer m_pointer = nullptr;

	public:
		RandomIterator( ) = default;

		explicit constexpr RandomIterator( T *ptr ) noexcept
		  : m_pointer{ ptr } {}

		constexpr RandomIterator &operator=( T *rhs ) noexcept {
			m_pointer = rhs;
			return *this;
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

		[[nodiscard]] constexpr reference operator*( ) {
			return *m_pointer;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *m_pointer;
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

		constexpr RandomIterator operator+( std::ptrdiff_t const &n ) noexcept {
			auto old = this->m_ptr;
			this->m_ptr += n;
			auto temp{ *this };
			this->m_ptr = old;
			return temp;
		}

		constexpr RandomIterator operator-( std::ptrdiff_t const &n ) noexcept {
			auto old = this->m_ptr;
			this->m_ptr -= n;
			auto temp{ *this };
			this->m_ptr = old;
			return temp;
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
	[[nodiscard]] constexpr auto make_random_iterator( T *const ptr ) noexcept {
		return RandomIterator<T>{ ptr };
	}

	template<typename T>
	[[nodiscard]] constexpr auto
	make_const_random_iterator( T *const ptr ) noexcept {
		return RandomIterator<T const>{ ptr };
	}
} // namespace daw

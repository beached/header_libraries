// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>
#include <iterator>
#include <memory>

namespace daw {
	template<typename T>
	struct prop_const_ptr {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using difference_type = std::ptrdiff_t;
		using pointer = T *;
		using const_pointer = T const *;
		using iterator_category = std::random_access_iterator_tag;

	private:
		pointer m_ptr;

	public:
		constexpr prop_const_ptr( pointer ptr )
		  : m_ptr( ptr ) {
			assert( ptr );
		}

		template<typename U,
		         std::enable_if_t<(not std::is_const_v<U> and std::is_const_v<T>),
		                          std::nullptr_t> = nullptr>
		constexpr prop_const_ptr( prop_const_ptr<U> const &other )
		  : prop_const_ptr( other.get( ) ) {}

		template<typename U,
		         std::enable_if_t<(not std::is_const_v<U> and std::is_const_v<T>),
		                          std::nullptr_t> = nullptr>
		constexpr prop_const_ptr &operator=( prop_const_ptr<U> const &rhs ) {
			m_ptr = rhs.get( );
			return *this;
		}

		constexpr operator prop_const_ptr<value_type const>( ) const {
			return { m_ptr };
		}

		constexpr pointer operator->( ) {
			return m_ptr;
		}

		constexpr const_pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) {
			return *m_ptr;
		}

		constexpr const_reference operator*( ) const {
			return *m_ptr;
		}

		constexpr reference operator[]( difference_type idx ) {
			return m_ptr[idx];
		}

		constexpr const_reference operator[]( difference_type idx ) const {
			return m_ptr[idx];
		}

		constexpr prop_const_ptr &operator++( ) {
			++m_ptr;
			return *this;
		}

		constexpr prop_const_ptr &operator++( int ) {
			auto result = *this;
			++m_ptr;
			return result;
		}

		constexpr prop_const_ptr &operator--( ) {
			--m_ptr;
			return *this;
		}

		constexpr prop_const_ptr &operator--( int ) {
			auto result = *this;
			--m_ptr;
			return result;
		}

		constexpr prop_const_ptr &operator+=( difference_type d ) {
			m_ptr += d;
			return *this;
		}

		constexpr prop_const_ptr &operator-=( difference_type d ) {
			m_ptr -= d;
			return *this;
		}

		constexpr prop_const_ptr operator+( difference_type d ) {
			auto result = *this;
			result += d;
			return result;
		}

		constexpr prop_const_ptr operator+( difference_type d ) const {
			auto result = prop_const_ptr<std::add_const_t<value_type>>( m_ptr );
			result += d;
			return result;
		}

		constexpr pointer get( ) {
			return m_ptr;
		}

		constexpr const_pointer get( ) const {
			return m_ptr;
		}

		template<typename U>
		constexpr difference_type operator-( prop_const_ptr<U> const &rhs ) const {
			return get( ) - rhs.get( );
		}

		friend constexpr bool operator==( prop_const_ptr const &lhs,
		                                  prop_const_ptr const &rhs ) {
			return lhs.m_ptr == rhs.m_ptr;
		}

		friend constexpr bool operator!=( prop_const_ptr const &lhs,
		                                  prop_const_ptr const &rhs ) {
			return lhs.m_ptr != rhs.m_ptr;
		}

		friend constexpr bool operator<( prop_const_ptr const &lhs,
		                                 prop_const_ptr const &rhs ) {
			return lhs.m_ptr < rhs.m_ptr;
		}

		friend constexpr bool operator<=( prop_const_ptr const &lhs,
		                                  prop_const_ptr const &rhs ) {
			return lhs.m_ptr <= rhs.m_ptr;
		}

		friend constexpr bool operator>( prop_const_ptr const &lhs,
		                                 prop_const_ptr const &rhs ) {
			return lhs.m_ptr > rhs.m_ptr;
		}

		friend constexpr bool operator>=( prop_const_ptr const &lhs,
		                                  prop_const_ptr const &rhs ) {
			return lhs.m_ptr >= rhs.m_ptr;
		}
	};
} // namespace daw

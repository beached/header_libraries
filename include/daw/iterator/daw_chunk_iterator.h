// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"
#include "../daw_move.h"

#include <cstddef>
#include <iterator>

namespace daw {
	template<typename Iterator>
	struct chunk_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type =
		  typename std::iterator_traits<Iterator>::difference_type;
		using size_type = std::size_t;
		using pointer = typename std::iterator_traits<Iterator>::pointer;
		using reference = typename std::iterator_traits<Iterator>::reference;

	private:
		Iterator m_iterator{ };
		difference_type m_chunk_size = 0;

	public:
		constexpr chunk_iterator( ) = default;

		constexpr chunk_iterator( Iterator iterator, size_type chunk_size )
		  : m_iterator( std::move( iterator ) )
		  , m_chunk_size( static_cast<difference_type>( chunk_size ) ) {}

		[[nodiscard]] constexpr decltype( auto ) operator*( ) {
			return *m_iterator;
		}

		[[nodiscard]] constexpr decltype( auto ) operator*( ) const {
			return *m_iterator;
		}

		[[nodiscard]] constexpr decltype( auto ) operator->( ) {
			return m_iterator->operator->( );
		}

		[[nodiscard]] constexpr decltype( auto ) operator->( ) const {
			return m_iterator->operator->( );
		}

		[[nodiscard]] constexpr decltype( auto ) operator[]( size_type idx ) {
			return m_iterator[idx];
		}

		[[nodiscard]] constexpr decltype( auto ) operator[]( size_type idx ) const {
			return m_iterator[idx];
		}

		constexpr chunk_iterator &operator++( ) {
			m_iterator += m_chunk_size;
			return *this;
		}

		constexpr chunk_iterator &operator++( int ) {
			auto result = *this;
			m_iterator += m_chunk_size;
			return result;
		}

		constexpr chunk_iterator &operator--( ) {
			m_iterator -= m_chunk_size;
			return *this;
		}

		constexpr chunk_iterator &operator--( int ) {
			auto result = *this;
			m_iterator -= m_chunk_size;
			return result;
		}

		constexpr chunk_iterator &operator+=( difference_type n ) {
			m_iterator += n * m_chunk_size;
			return *this;
		}

		constexpr chunk_iterator &operator-=( difference_type n ) {
			m_iterator += n * m_chunk_size;
			return *this;
		}

		[[nodiscard]] friend constexpr chunk_iterator
		operator+( difference_type n, chunk_iterator it ) {
			it += n;
			return it;
		}

		[[nodiscard]] friend constexpr chunk_iterator
		operator+( chunk_iterator it, difference_type n ) {
			it += n;
			return it;
		}

		[[nodiscard]] friend constexpr chunk_iterator
		operator-( difference_type n, chunk_iterator &it ) {
			it -= n;
			return it;
		}

		[[nodiscard]] friend constexpr chunk_iterator
		operator-( chunk_iterator it, difference_type n ) {
			it -= n;
			return it;
		}

		[[nodiscard]] constexpr difference_type
		operator-( chunk_iterator const &rhs ) const {
			return m_iterator - rhs.m_iterator;
		}

		constexpr bool operator==( chunk_iterator const &rhs ) const {
			return m_iterator == rhs.m_iterator;
		}

		constexpr bool operator!=( chunk_iterator const &rhs ) const {
			return m_iterator != rhs.m_iterator;
		}

		constexpr bool operator<( chunk_iterator const &rhs ) const {
			return m_iterator < rhs.m_iterator;
		}

		constexpr bool operator<=( chunk_iterator const &rhs ) const {
			return m_iterator <= rhs.m_iterator;
		}

		constexpr bool operator>( chunk_iterator const &rhs ) const {
			return m_iterator > rhs.m_iterator;
		}

		constexpr bool operator>=( chunk_iterator const &rhs ) const {
			return m_iterator >= rhs.m_iterator;
		}
	};
	template<typename Iterator>
	chunk_iterator( Iterator, std::size_t ) -> chunk_iterator<Iterator>;
} // namespace daw

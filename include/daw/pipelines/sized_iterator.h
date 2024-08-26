// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_ensure.h"
#include "daw/daw_iterator_traits.h"

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	template<Iterator Iterator>
	struct sized_iterator;

	template<ForwardIterator Iterator>
	struct sized_iterator<Iterator> {
		using iterator_category = iter_category_t<Iterator>;
		using value_type = iter_value_t<Iterator>;
		using reference = iter_reference_t<Iterator>;
		using const_reference = iter_const_reference_t<Iterator>;
		using difference_type = std::ptrdiff_t;

	private:
		Iterator m_iter = Iterator{ };
		difference_type m_count = 0;

		constexpr void increment( ) {
			--m_count;
			++m_iter;
#if defined( DEBUG ) or not defined( NDEBUG )
			daw_ensure( m_count >= 0 );
#endif
		}

		constexpr void decrement( ) {
			++m_count;
			--m_iter;
		}

		constexpr void advance( difference_type n ) {
			m_count -= std::min( { n, m_count } );
			std::advance( m_iter, n );
		}

	public:
		explicit sized_iterator( ) = default;

		explicit constexpr sized_iterator( Iterator first, std::size_t how_many )
		  : m_iter( std::move( first ) )
		  , m_count( static_cast<difference_type>( how_many ) ) {}

		[[nodiscard]] constexpr auto &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_iter;
		}

		DAW_ATTRIB_INLINE constexpr sized_iterator &operator++( ) {
			increment( );
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr sized_iterator operator++( int ) {
			auto tmp = *this;
			increment( );
			return tmp;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) noexcept {
			return *m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const noexcept {
			return *m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator==( sized_iterator const &rhs ) const noexcept {
			return m_count == rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator!=( sized_iterator const &rhs ) const noexcept {
			return m_count != rhs.m_count;
		}

		// bidirectional iterator interface
		constexpr sized_iterator &operator--( )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			decrement( );
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr sized_iterator operator--( int )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			auto tmp = *this;
			decrement( );
			return tmp;
		}

		// random access iterator interface
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator[]( difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iter[n];
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator[]( difference_type n ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iter[n];
		}

		constexpr sized_iterator &operator+=( difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			advance( n );
			return *this;
		}

		constexpr sized_iterator &operator-=( difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			advance( -n );
			return *this;
		}

		constexpr sized_iterator operator+( difference_type n ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			auto result = *this;
			result.advance( n );
			return result;
		}

		constexpr sized_iterator operator-( difference_type n ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			auto result = *this;
			result.advance( -n );
			return result;
		}

		constexpr difference_type operator-( sized_iterator const &rhs ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return rhs.m_count - m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator<( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_count > rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator<=( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_count >= rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator>( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_count < rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator>=( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_count <= rhs.m_count;
		}
	};

	template<InputIterator Iterator>
	struct sized_iterator<Iterator> {
		using iterator_category = std::input_iterator_tag;
		using value_type = iter_value_t<Iterator>;
		using reference = iter_reference_t<Iterator>;
		using const_reference = iter_const_reference_t<Iterator>;
		using difference_type = std::ptrdiff_t;

	private:
		Iterator m_first = Iterator{ };
		Iterator m_last = Iterator{ };
		difference_type m_count = 0;

		constexpr void increment( ) {
			--m_count;
			++m_first;
			if( m_first == m_last ) {
				m_count = 0;
			}
		}

	public:
		explicit sized_iterator( ) = default;

		explicit constexpr sized_iterator( Iterator first,
		                                   Iterator last,
		                                   std::size_t how_many )
		  : m_first( first )
		  , m_last( last )
		  , m_count( static_cast<difference_type>( how_many ) ) {}

		[[nodiscard]] constexpr auto &base( ) {
			return m_first;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_first;
		}

		DAW_ATTRIB_INLINE constexpr sized_iterator &operator++( ) {
			increment( );
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr sized_iterator operator++( int ) {
			auto tmp = *this;
			increment( );
			return tmp;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) noexcept {
			return *m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const noexcept {
			return *m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator==( sized_iterator const &rhs ) const noexcept {
			return m_count == rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator!=( sized_iterator const &rhs ) const noexcept {
			return m_count != rhs.m_count;
		}
	};

} // namespace daw::pipelines

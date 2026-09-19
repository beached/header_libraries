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
	template<Iterator SentinelFor>
	struct sized_iterator_end {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<SentinelFor>>;
		using difference_type = std::ptrdiff_t;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_sized_iterator_end_class = void;

		sized_iterator_end( ) = default;

		constexpr bool operator==( sized_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline sized_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline sized_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<Iterator First, Iterator Last = First>
	struct sized_iterator;

	template<RandomIterator First, Iterator Last>
	struct sized_iterator<First, Last> {
		using iterator_category = iter_category_t<First>;
		using value_type = iter_value_t<First>;
		using reference = iter_reference_t<First>;
		using const_reference = iter_const_reference_t<First>;
		using difference_type = std::ptrdiff_t;

	private:
		First m_iter{ };
		difference_type m_count{ };

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
		sized_iterator( ) = default;

		explicit constexpr sized_iterator( daw::constructible<First> auto &&first,
		                                   std::size_t how_many )
		  : m_iter( DAW_FWD( first ) )
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
			return m_count == rhs.m_count or m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr bool
		operator==( sized_iterator_end<First> const & ) const noexcept {
			return m_count <= 0;
		}

		[[nodiscard]] constexpr bool
		operator==( sized_iterator_end<Last> const & ) const noexcept
		  requires( not std::same_as<First, Last> ) {
			return m_count <= 0;
		}

		// clang-format off
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator<=>( sized_iterator const &rhs ) const noexcept {
			return m_count <=> rhs.m_count;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator<=>( sized_iterator_end<First> const & ) const noexcept {
			return m_count <=> 0;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator<=>( sized_iterator_end<Last> const & ) const noexcept
		  requires( not std::same_as<First, Last> ) {
			return m_count <=> 0;
		}
		// clang-format on

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

		friend constexpr sized_iterator operator+( sized_iterator lhs,
		                                           difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			lhs += n;
			return lhs;
		}

		friend constexpr sized_iterator operator+( difference_type n,
		                                           sized_iterator rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			rhs += n;
			return rhs;
		}

		friend constexpr sized_iterator operator-( sized_iterator lhs,
		                                           difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			lhs -= n;
			return lhs;
		}

		friend constexpr sized_iterator operator-( difference_type n,
		                                           sized_iterator rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			rhs -= n;
			return rhs;
		}

		constexpr difference_type operator-( sized_iterator const &rhs ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return rhs.m_count - m_count;
		}
	};

	template<InputIterator First, Iterator Last>
	struct sized_iterator<First, Last> {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<First>>;
		using value_type = iter_value_t<First>;
		using reference = iter_reference_t<First>;
		using const_reference = iter_const_reference_t<First>;
		using difference_type = std::ptrdiff_t;

	private:
		First m_first = First{ };
		Last m_last = Last{ };
		difference_type m_count = 0;

		constexpr void increment( ) {
			--m_count;
			++m_first;
			if( m_first == m_last ) {
				m_count = 0;
			}
		}

	public:
		sized_iterator( ) = default;

		explicit constexpr sized_iterator( daw::constructible<First> auto &&first,
		                                   daw::constructible<Last> auto &&last,
		                                   std::size_t how_many )
		  : m_first( DAW_FWD( first ) )
		  , m_last( DAW_FWD( last ) )
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

		[[nodiscard]] constexpr bool
		operator==( sized_iterator_end<First> const & ) const noexcept {
			return m_count <= 0;
		}

		[[nodiscard]] constexpr bool
		operator==( sized_iterator_end<Last> const & ) const noexcept
		  requires( not std::same_as<First, Last> ) {
			return m_count <= 0;
		}
	};
} // namespace daw::pipelines

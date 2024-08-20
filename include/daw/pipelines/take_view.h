// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_typeof.h"
#include "range.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>

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

	namespace pimple {
		struct Take_t {
			std::size_t how_many_to_skip = 0;
			std::size_t how_many = 0;

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				using iter_t = iterator_t<R>;
				auto first = std::begin( r );
				auto last = std::end( r );

				if constexpr( ForwardIterator<iter_t> ) {
					auto const range_size = std::distance( first, last );
					auto const skip = std::min(
					  { static_cast<std::ptrdiff_t>( how_many_to_skip ), range_size } );
					first = std::next( first, skip );
					auto take_size = std::min(
					  { range_size - skip, static_cast<std::ptrdiff_t>( how_many ) } );
					return range_t{ sized_iterator<iter_t>{
					                  first, static_cast<std::size_t>( take_size ) },
					                sized_iterator<iter_t>{ last, 0 } };
				} else {
					// Input
					for( auto n = how_many_to_skip; n > 0; --n ) {
						if( first == last ) {
							break;
						}
						++first;
					}
					return range_t{ sized_iterator<iter_t>{ first, how_many },
					                sized_iterator<iter_t>{ last, 0 } };
				}
			}
		};
	} // namespace pimple

	[[nodiscard]] constexpr auto Take( std::size_t how_many ) {
		return pimple::Take_t{ 0, how_many };
	}

	[[nodiscard]] constexpr auto Take( std::size_t how_many_to_skip,
	                                   std::size_t how_many ) {
		return pimple::Take_t{ how_many_to_skip, how_many };
	}
} // namespace daw::pipelines

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

namespace daw::pipelines {
	template<typename Iterator>
	struct sized_iterator {
		using iterator_category = iter_category_t<Iterator>;
		using value_type = iter_value_t<Iterator>;
		using reference = iter_reference_t<Iterator>;
		using const_reference = iter_const_reference_t<Iterator>;
		using difference_type = std::ptrdiff_t;
		static_assert( ForwardIterator<Iterator>,
		               "Must supply at least forward iterator types" );

	private:
		Iterator m_iter = Iterator{ };
		difference_type m_position = 0;

		constexpr void increment( ) {
			++m_position;
			++m_iter;
		}

		constexpr void decrement( ) {
			--m_position;
			--m_iter;
		}

		constexpr void advance( difference_type n ) {
			m_position += n;
			std::advance( m_iter, n );
		}

	public:
		explicit sized_iterator( ) = default;

		explicit constexpr sized_iterator( Iterator it, std::size_t pos )
		  : m_iter( std::move( it ) )
		  , m_position( static_cast<difference_type>( pos ) ) {}

		explicit constexpr sized_iterator( std::size_t pos )
		  : m_position( static_cast<difference_type>( pos ) ) {}

		constexpr sized_iterator &operator++( ) {
			increment( );
			return *this;
		}

		[[nodiscard]] constexpr sized_iterator operator++( int ) {
			auto tmp = *this;
			increment( );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			return *m_iter;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			return *m_iter;
		}

		[[nodiscard]] constexpr bool
		operator==( sized_iterator const &rhs ) const noexcept {
			return m_position == rhs.m_position;
		}

		[[nodiscard]] constexpr bool
		operator!=( sized_iterator const &rhs ) const noexcept {
			return m_position != rhs.m_position;
		}

		// bidirectional iterator interface
		constexpr sized_iterator &operator--( )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			decrement( );
			return *this;
		}

		[[nodiscard]] constexpr sized_iterator operator--( int )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			auto tmp = *this;
			decrement( );
			return tmp;
		}

		// random access iterator interface
		[[nodiscard]] constexpr reference operator[]( difference_type n )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iter[n];
		}

		[[nodiscard]] constexpr reference operator[]( difference_type n ) const
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
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] constexpr bool operator<( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_position < rhs.m_position;
		}

		[[nodiscard]] constexpr bool operator<=( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_position <= rhs.m_position;
		}

		[[nodiscard]] constexpr bool operator>( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_position > rhs.m_position;
		}

		[[nodiscard]] constexpr bool operator>=( sized_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_position >= rhs.m_position;
		}
	};
	namespace pimple {
		template<ForwardIterator It, typename Last>
		constexpr It
		safe_next( It first, Last const &last, std::size_t how_many = 1U ) {
			auto const max_how_many = std::distance( first, last );
			auto const h =
			  std::min( static_cast<std::ptrdiff_t>( how_many ), max_how_many );
			return std::next( last, h );
		}

		struct Take_t {
			std::size_t how_many_to_skip = 0;
			std::size_t how_many = 0;

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				auto first =
				  how_many_to_skip == 0
				    ? std::begin( r )
				    : safe_next( std::begin( r ), std::end( r ), how_many_to_skip );
				using iter_t = iterator_t<R>;
				return range_t{ sized_iterator<iter_t>{ first, 0 },
				                sized_iterator<iter_t>{ first, how_many } };
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

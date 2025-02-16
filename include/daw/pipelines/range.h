// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/pipelines/range_base.h"

#include <cstddef>
#include <daw/stdinc/tuple_traits.h>
#include <iterator>
#include <type_traits>

namespace daw::pipelines {
	template<Iterator First, Iterator Last = First>
	struct range_t : range_base_t<First, Last> {
		using i_am_a_daw_pipelines_range_t = void;
		using iterator_category =
		  common_iterator_category_t<iter_category_t<First>, iter_category_t<Last>>;
		using reference = iter_reference_t<First>;
		using value_type = iter_value_t<First>;
		using const_reference = iter_const_reference_t<First>;
		using difference_type =
		  typename std::iterator_traits<First>::difference_type;
		using pointer = iter_pointer_t<First>;

		First first;
		Last last;

		range_t( ) = default;
		constexpr range_t( First f, Last l )
		  : first( std::move( f ) )
		  , last( std::move( l ) ) {}

		template<Range R>
		constexpr range_t( R &&r )
		  : first( std::begin( r ) )
		  , last( std::end( r ) ) {}

		[[nodiscard]] constexpr First begin( ) {
			return first;
		}

		[[nodiscard]] constexpr First begin( ) const {
			return first;
		}

		[[nodiscard]] constexpr First cbegin( ) const {
			return first;
		}

		[[nodiscard]] constexpr Last end( ) {
			return last;
		}

		[[nodiscard]] constexpr Last end( ) const {
			return last;
		}

		[[nodiscard]] constexpr Last cend( ) const {
			return last;
		}

		[[nodiscard]] constexpr std::size_t size( ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return static_cast<std::size_t>( std::distance( first, last ) );
		}

		[[nodiscard]] constexpr auto data( )
		  requires( std::contiguous_iterator<First> ) {
			return std::to_address( first );
		}

		[[nodiscard]] constexpr auto data( ) const
		  requires( std::contiguous_iterator<First> ) {
			return std::to_address( first );
		}

		[[nodiscard]] constexpr auto data_end( )
		  requires( std::contiguous_iterator<First> ) {
			return std::to_address( last );
		}

		[[nodiscard]] constexpr auto data_end( ) const
		  requires( std::contiguous_iterator<First> ) {
			return std::to_address( last );
		}

		constexpr range_t &operator++( ) {
			++first;
			return *this;
		}

		[[nodiscard]] range_t operator++( int ) {
			auto result = *this;
			++first;
			return result;
		}

		constexpr range_t &operator--( )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			--first;
			return *this;
		}

		constexpr range_t operator--( int )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			auto result = *this;
			--first;
			return result;
		}

		constexpr reference operator[]( difference_type idx )
		  requires( RandomIteratorTag<iterator_category> ) {
			return first[idx];
		}

		constexpr const_reference operator[]( difference_type idx ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return first[idx];
		}

		constexpr reference operator*( ) {
			return *first;
		}

		constexpr const_reference operator*( ) const {
			return *first;
		}

		constexpr pointer operator->( ) {
			if constexpr( std::is_pointer_v<First> ) {
				return first;
			} else {
				return first.operator->( );
			}
		}

		constexpr auto operator->( ) const {
			if constexpr( std::is_pointer_v<First> ) {
				return first;
			} else {
				return first.operator->( );
			}
		}

		constexpr difference_type operator-( range_t const &rhs ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return std::distance( first, rhs.first );
		}

		constexpr bool operator==( range_t const &rhs ) const = default;
		constexpr bool operator!=( range_t const &rhs ) const = default;
		// clang-format off
		constexpr auto operator<=>( range_t const &rhs ) const = default;
		// clang-format on
	};
	template<typename I, typename L>
	range_t( I, L ) -> range_t<I, L>;

	template<Range R>
	range_t( R && ) -> range_t<iterator_t<R>, iterator_end_t<R>>;
} // namespace daw::pipelines

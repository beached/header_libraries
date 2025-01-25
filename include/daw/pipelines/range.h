// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <iterator>

namespace daw::pipelines {
	template<Iterator First, Iterator Last = First>
	struct range_t {
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
} // namespace daw::pipelines

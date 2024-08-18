// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/iterator/daw_arrow_proxy.h"

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace daw::pipelines {
	template<typename T>
	struct iota_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using reference = value_type;
		using const_reference = value_type;
		using pointer = daw::arrow_proxy<value_type>;
		using const_pointer = daw::arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		T value = std::numeric_limits<T>::max( );

	public:
		explicit iota_iterator( ) = default;

		explicit constexpr iota_iterator( T const &v )
		  : value( v ) {}

		explicit constexpr iota_iterator( T &&v )
		  : value( std::move( v ) ) {}

		[[nodiscard]] constexpr value_type operator*( ) const {
			return value;
		}

		[[nodiscard]] constexpr daw::arrow_proxy<value_type> operator->( ) const {
			return daw::arrow_proxy<value_type>( operator*( ) );
		}

		constexpr iota_iterator &operator++( ) {
			++value;
			return *this;
		}

		[[nodiscard]] constexpr iota_iterator operator++( int ) {
			iota_iterator result = *this;
			++value;
			return result;
		}

		constexpr iota_iterator &operator--( ) {
			--value;
			return *this;
		}

		[[nodiscard]] constexpr iota_iterator operator--( int ) {
			iota_iterator result = *this;
			--value;
			return result;
		}

		constexpr iota_iterator &operator+=( difference_type n ) {
			value += static_cast<value_type>( n );
			return *this;
		}

		constexpr iota_iterator &operator-=( difference_type n ) {
			value -= static_cast<value_type>( n );
			return *this;
		}

		[[nodiscard]] constexpr iota_iterator
		operator+( difference_type n ) const noexcept {
			iota_iterator result = *this;
			result.value += n;
			return result;
		}

		[[nodiscard]] constexpr iota_iterator
		operator-( difference_type n ) const noexcept {
			iota_iterator result = *this;
			result.value -= n;
			return result;
		}

		[[nodiscard]] constexpr difference_type
		operator-( iota_iterator const &i ) const {
			return static_cast<difference_type>( value - i.value );
		}

		[[nodiscard]] constexpr reference operator[]( size_type n ) noexcept {
			return *( value + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type n ) const noexcept {
			return *( value + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] friend constexpr bool operator==( iota_iterator const &lhs,
		                                                iota_iterator const &rhs ) {
			return lhs.value == rhs.value;
		}

		[[nodiscard]] friend constexpr bool operator!=( iota_iterator const &lhs,
		                                                iota_iterator const &rhs ) {
			return lhs.value != rhs.value;
		}

		[[nodiscard]] friend constexpr bool operator<( iota_iterator const &lhs,
		                                               iota_iterator const &rhs ) {
			return lhs.value < rhs.value;
		}

		[[nodiscard]] friend constexpr bool operator<=( iota_iterator const &lhs,
		                                                iota_iterator const &rhs ) {
			return lhs.value <= rhs.value;
		}

		[[nodiscard]] friend constexpr bool operator>( iota_iterator const &lhs,
		                                               iota_iterator const &rhs ) {
			return lhs.value > rhs.value;
		}

		[[nodiscard]] friend constexpr bool operator>=( iota_iterator const &lhs,
		                                                iota_iterator const &rhs ) {
			return lhs.value >= rhs.value;
		}
	};

	template<typename T>
	struct iota_view {
		using value_type = daw::iter_value_t<iota_iterator<T>>;
		using iterator = iota_iterator<T>;

	private:
		iota_iterator<T> m_first = iota_iterator<T>{ };
		iota_iterator<T> m_last = iota_iterator<T>{ };

	public:
		explicit iota_view( ) = default;

		explicit constexpr iota_view( T last )
		  : m_first( 0 )
		  , m_last( last ) {}

		explicit constexpr iota_view( T first, T last )
		  : m_first( first )
		  , m_last( last ) {}

		[[nodiscard]] constexpr iterator begin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return m_last;
		}
	};
	template<typename T>
	iota_view( T ) -> iota_view<std::size_t>;

	template<typename T>
	iota_view( T, T ) -> iota_view<std::size_t>;

	inline constexpr auto ToIota = []<typename T>( T last ) {
		return iota_view<T>{ 0, last };
	};
} // namespace daw::pipelines

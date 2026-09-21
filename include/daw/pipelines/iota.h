// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_arith_traits.h"
#include "daw/daw_as.h"
#include "daw/daw_concepts.h"
#include "daw/daw_ensure.h"
#include "daw/daw_int_cmp.h"
#include "daw/daw_iterator_traits.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "daw/pipelines/range_base.h"

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
		using difference_type = daw::next_wider_t<daw::make_signed_t<T>>;
		using size_type = daw::next_wider_t<daw::make_unsigned_t<T>>;

	private:
		T m_value = max_value<T>;

	public:
		iota_iterator( ) = default;

		explicit constexpr iota_iterator(
		  daw::explicitly_convertible_to<T> auto &&value )
		  : m_value( as<T>( DAW_FWD( value ) ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) const {
			return m_value;
		}

		constexpr iota_iterator &operator++( ) {
			return *this += value_type{ 1 };
		}

		[[nodiscard]] constexpr iota_iterator operator++( int ) {
			iota_iterator result = *this;
			*this += value_type{ 1 };
			return result;
		}

		constexpr iota_iterator &operator--( ) {
			return *this -= value_type{ 1 };
		}

		[[nodiscard]] constexpr iota_iterator operator--( int ) {
			iota_iterator result = *this;
			*this -= value_type{ 1 };
			return result;
		}

		DAW_ATTRIB_INLINE constexpr iota_iterator &operator+=( difference_type n ) {
			// size_type is the unsigned type as wide as difference_type.  Adding in
			// it wraps, where a signed sum could overflow for a large step
			m_value = as<T>( as<size_type>( m_value ) + as<size_type>( n ) );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr iota_iterator &operator-=( difference_type n ) {
			m_value = as<T>( as<size_type>( m_value ) - as<size_type>( n ) );
			return *this;
		}

		[[nodiscard]] constexpr friend iota_iterator
		operator+( iota_iterator lhs, difference_type n ) noexcept {
			lhs += n;
			return lhs;
		}

		[[nodiscard]] constexpr friend iota_iterator
		operator+( difference_type n, iota_iterator rhs ) noexcept {
			rhs += n;
			return rhs;
		}

		[[nodiscard]] constexpr friend iota_iterator
		operator-( iota_iterator lhs, difference_type n ) noexcept {
			lhs -= n;
			return lhs;
		}

		[[nodiscard]] constexpr difference_type
		operator-( iota_iterator const &i ) const {
			return as<difference_type>( m_value ) - as<difference_type>( i.m_value );
		}

		DAW_ATTRIB_FLATTEN [[nodiscard]] constexpr reference
		operator[]( difference_type n ) const noexcept {
			return *( *this + n );
		}

		[[nodiscard]] constexpr bool
		operator==( iota_iterator const &rhs ) const = default;

		// clang-format off
		[[nodiscard]] friend constexpr auto operator<=>
		  ( iota_iterator const &lhs, iota_iterator const &rhs ) {
			return lhs.m_value <=> rhs.m_value;
		}
		// clang-format on
	};

	template<typename T>
	struct iota_view {
		using iterator = iota_iterator<T>;
		using difference_type = daw::iter_difference_t<iterator>;
		using value_type = daw::iter_value_t<iterator>;
		using size_type = daw::iter_size_t<iterator>;

	private:
		T m_first = { };
		T m_last = max_value<T>;

	public:
		explicit iota_view( ) = default;

		explicit constexpr iota_view(
		  daw::explicitly_convertible_to<T> auto &&last )
		  : m_last{ as<T>( DAW_FWD( last ) ) } {
			daw_dbg_ensure( m_first <= m_last );
		}

		explicit constexpr iota_view(
		  daw::explicitly_convertible_to<T> auto &&first,
		  daw::explicitly_convertible_to<T> auto &&last )
		  : m_first{ as<T>( DAW_FWD( first ) ) }
		  , m_last{ as<T>( DAW_FWD( last ) ) } {
			daw_dbg_ensure( m_first <= m_last );
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			return iterator{ m_first };
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return iterator{ m_last };
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_first == m_last;
		}

		DAW_ATTRIB_FLATTEN [[nodiscard]] constexpr value_type
		operator[]( difference_type n ) const noexcept {
			return begin( )[n];
		}

		[[nodiscard]] constexpr bool operator==( iota_view const &rhs ) const {
			return m_first == rhs.m_first and m_last == rhs.m_last;
		}
	};
	template<typename T>
	iota_view( T ) -> iota_view<T>;

	template<typename T>
	iota_view( T, T ) -> iota_view<T>;

	template<typename T>
	struct countdown_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = daw::next_wider_t<daw::make_signed_t<T>>;
		using size_type = daw::next_wider_t<daw::make_unsigned_t<T>>;

	private:
		value_type m_from = T{ };

	public:
		countdown_iterator( ) = default;

		explicit constexpr countdown_iterator(
		  daw::explicitly_convertible_to<value_type> auto &&from )
		  : m_from( as<value_type>( DAW_FWD( from ) ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) const {
			return m_from;
		}

		constexpr countdown_iterator &operator++( ) {
			return *this += value_type{ 1 };
		}

		[[nodiscard]] constexpr countdown_iterator operator++( int ) {
			countdown_iterator result = *this;
			*this += value_type{ 1 };
			return result;
		}

		constexpr countdown_iterator &operator--( ) {
			return *this -= value_type{ 1 };
		}

		[[nodiscard]] constexpr countdown_iterator operator--( int ) {
			countdown_iterator result = *this;
			*this -= value_type{ 1 };
			return result;
		}

		DAW_ATTRIB_INLINE constexpr countdown_iterator &
		operator+=( difference_type n ) {
			m_from = as<value_type>( as<size_type>( m_from ) - as<size_type>( n ) );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr countdown_iterator &
		operator-=( difference_type n ) {
			m_from = as<value_type>( as<size_type>( m_from ) + as<size_type>( n ) );
			return *this;
		}

		[[nodiscard]] constexpr friend countdown_iterator
		operator+( countdown_iterator lhs, difference_type n ) noexcept {
			lhs += n;
			return lhs;
		}

		[[nodiscard]] constexpr friend countdown_iterator
		operator+( difference_type n, countdown_iterator rhs ) noexcept {
			rhs += n;
			return rhs;
		}

		[[nodiscard]] constexpr friend countdown_iterator
		operator-( countdown_iterator lhs, difference_type n ) noexcept {
			lhs -= n;
			return lhs;
		}

		[[nodiscard]] constexpr difference_type
		operator-( countdown_iterator const &i ) const {
			// the value falls as the iterator advances
			return as<difference_type>( i.m_from ) - as<difference_type>( m_from );
		}

		DAW_ATTRIB_FLATTEN [[nodiscard]] constexpr reference
		operator[]( difference_type n ) const noexcept {
			return *( *this + n );
		}

		[[nodiscard]] constexpr bool
		operator==( countdown_iterator const &rhs ) const = default;

		// clang-format off
		[[nodiscard]] friend constexpr auto operator<=>
		  ( countdown_iterator const &lhs, countdown_iterator const &rhs ) {
			return rhs.m_from <=> lhs.m_from;
		}
		// clang-format on
	};
	template<typename T>
	struct countdown_view {
		using iterator = countdown_iterator<T>;
		using difference_type = daw::iter_difference_t<iterator>;
		using value_type = daw::iter_value_t<iterator>;
		using size_type = daw::iter_size_t<iterator>;

	private:
		T m_from = max_value<T>;
		T m_to = T{ };

	public:
		explicit countdown_view( ) = default;

		explicit constexpr countdown_view(
		  daw::explicitly_convertible_to<T> auto &&from )
		  : m_from{ as<T>( DAW_FWD( from ) ) } {
			daw_dbg_ensure( m_from >= m_to );
		}

		explicit constexpr countdown_view(
		  daw::explicitly_convertible_to<T> auto &&first,
		  daw::explicitly_convertible_to<T> auto &&last )
		  : m_from{ as<T>( DAW_FWD( first ) ) }
		  , m_to{ as<T>( DAW_FWD( last ) ) } {
			daw_dbg_ensure( m_from >= m_to );
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			return iterator{ m_from };
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return iterator{ m_to };
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_from == m_to;
		}

		DAW_ATTRIB_FLATTEN [[nodiscard]] constexpr value_type
		operator[]( difference_type n ) const noexcept {
			return begin( )[n];
		}

		[[nodiscard]] constexpr bool operator==( countdown_view const &rhs ) const {
			return m_from == rhs.m_from and m_to == rhs.m_to;
		}
	};
	template<typename T>
	countdown_view( T ) -> countdown_view<T>;

	template<typename T>
	countdown_view( T, T ) -> countdown_view<T>;

	inline constexpr auto ToIota = []<Integer I>( I last ) {
		return iota_view<I>{ I{ }, last };
	};
} // namespace daw::pipelines

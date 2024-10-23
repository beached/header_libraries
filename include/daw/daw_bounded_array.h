// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_algorithm.h"

#include <cstddef>
#include <cstdint>

namespace daw {
	template<typename T, size_t N>
	struct array {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = value_type *;
		using const_iterator = value_type const *;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = size_t;

		value_type m_data[N];

		[[nodiscard]] constexpr reference front( ) noexcept {
			return m_data[0];
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			return m_data[0];
		}

		[[nodiscard]] constexpr reference back( ) noexcept {
			return m_data[N - 1];
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			return m_data[N - 1];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_data;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_data;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return m_data;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_data;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_data;
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return m_data + static_cast<intmax_t>( N );
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return m_data + static_cast<intmax_t>( N );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return m_data + static_cast<intmax_t>( N );
		}

		constexpr void fill( const_reference value ) noexcept {
			daw::algorithm::fill_n( m_data, N, value );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return N == 0;
		}

		[[nodiscard]] constexpr reference operator[]( size_type pos ) noexcept {
			return m_data[pos];
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type pos ) const noexcept {
			return m_data[pos];
		}
	}; // array

	template<typename T, typename U, size_t N>
	constexpr bool operator==( array<T, N> const &lhs, array<U, N> const &rhs ) {

		return daw::algorithm::equal( lhs.cbegin( ), lhs.cend( ), rhs.cbegin( ),
		                              rhs.cend( ) );
	}

	template<typename T, typename U, size_t N>
	constexpr bool operator!=( array<T, N> const &lhs, array<U, N> const &rhs ) {

		return not daw::algorithm::equal( lhs.cbegin( ), lhs.cend( ), rhs.cbegin( ),
		                                  rhs.cend( ) );
	}

	template<typename T, size_t N>
	using bounded_array_t = daw::array<T, N>;
} // namespace daw

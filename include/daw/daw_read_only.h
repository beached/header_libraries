// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <memory>
#include <type_traits>

namespace daw {
	template<typename T>
	struct read_only {
		static_assert( not std::is_reference_v<T>,
		               "Reference types are not supported" );
		static_assert( not std::is_const_v<T>, "Const types are not supported" );
		using value_type = T;

	private:
		value_type m_value;

	public:
		explicit read_only( ) = default;
		read_only( read_only const & ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> ) = default;
		read_only( read_only && ) noexcept(
		  std::is_nothrow_move_constructible_v<T> ) = default;
		read_only &operator=( read_only const & ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> ) = default;
		read_only &operator=( read_only && ) noexcept(
		  std::is_nothrow_move_constructible_v<T> ) = default;
		~read_only( ) = default;

		explicit constexpr read_only( T const &value ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> )
		  : m_value( value ) {}
		explicit constexpr read_only( T &&value ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : m_value( std::move( value ) ) {}

		[[nodiscard]] constexpr operator value_type const &( ) const noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr value_type const &get( ) const noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr value_type &read_write( ) noexcept {
			return m_value;
		}

		constexpr value_type const *operator->( ) const noexcept {
			return std::addressof( m_value );
		}

		[[nodiscard]] constexpr value_type
		move_out( ) noexcept( std::is_nothrow_move_constructible_v<T> ) {
			return std::move( m_value );
		}
	}; // read_only
} // namespace daw

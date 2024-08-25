// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	requires( not std::is_reference_v<T> and
	          not std::is_pointer_v<T> ) class move_only {

		T m_value;

	public:
		move_only( ) = default;

		~move_only( ) = default;

		move_only( move_only && ) = default;

		move_only &operator=( move_only && ) = default;

		move_only( move_only const & ) = delete;

		move_only &operator=( move_only const & ) = delete;

		// clang-format off
		auto operator<=> ( move_only const & ) const = default;
		// clang-format on

		explicit constexpr move_only( T const &value )
		  : m_value( value ) {}

		explicit constexpr move_only( T &&value )
		  : m_value( std::move( value ) ) {}

		constexpr T &get( ) {
			return m_value;
		}

		constexpr T const &get( ) const {
			return m_value;
		}

		explicit constexpr operator T &( ) {
			return m_value;
		}

		explicit constexpr operator T const &( ) const {
			return m_value;
		}

		constexpr auto *operator->( ) {
			return std::addressof( m_value );
		}

		constexpr auto const *operator->( ) const {
			return std::addressof( m_value );
		}
	};
	template<typename T>
	move_only( T ) -> move_only<T>;

	template<>
	class move_only<void> {
	public:
		move_only( ) = default;
		~move_only( ) = default;
		move_only( move_only && ) = default;
		move_only &operator=( move_only && ) = default;
		move_only( move_only const & ) = delete;
		move_only &operator=( move_only const & ) = delete;
	};
} // namespace daw

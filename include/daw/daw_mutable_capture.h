// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_move.h>

#include <memory>
#include <type_traits>

namespace daw {
	template<typename T>
	class mutable_capture {
		mutable T m_value;

	public:
		explicit constexpr mutable_capture( T const &value )
		  : m_value( value ) {}
		explicit constexpr mutable_capture( T &value )
		  : m_value( value ) {}
		explicit constexpr mutable_capture( T &&value )
		  : m_value( DAW_MOVE( value ) ) {}

		[[nodiscard]] constexpr T move_out( ) const
		  noexcept( std::is_nothrow_move_constructible_v<T> ) {
			return DAW_MOVE( m_value );
		}

		[[nodiscard]] constexpr T &get( ) const noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr T *ptr( ) const noexcept {
			return std::addressof( m_value );
		}

		[[nodiscard]] constexpr operator T &( ) const &noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr
		operator T( ) const &&noexcept( std::is_nothrow_move_constructible_v<T> ) {
			return DAW_MOVE( m_value );
		}

		[[nodiscard]] constexpr T &operator*( ) const &noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr T
		operator*( ) const &&noexcept( std::is_nothrow_move_constructible_v<T> ) {
			return DAW_MOVE( m_value );
		}

		[[nodiscard]] constexpr T *operator->( ) const noexcept {
			return &m_value;
		}
	};

	template<typename T>
	mutable_capture( T ) -> mutable_capture<T>;
} // namespace daw

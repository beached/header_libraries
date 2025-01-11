// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_apply.h"
#include "daw/daw_attributes.h"
#include "daw/daw_monadic_pointer_ops.h"

#include <compare>
#include <concepts>
#include <memory>

namespace daw {
	template<typename T>
	struct observer_ptr {
		using element_type = T;
		using pointer = element_type *;
		using reference = element_type &;

	private:
		pointer m_ptr = nullptr;

	public:
		observer_ptr( ) = default;
		constexpr observer_ptr( std::nullptr_t ) {}
		constexpr explicit observer_ptr( pointer p )
		  : m_ptr( p ) {}

		template<typename U>
		requires( not std::same_as<T, U> and
		          std::convertible_to<
		            U *, T *> ) constexpr observer_ptr( observer_ptr<U> other )
		  : m_ptr( other.m_ptr ) {}

		constexpr void release( ) {
			m_ptr = nullptr;
		}

		constexpr void reset( pointer p = nullptr ) {
			m_ptr = p;
		}

		constexpr pointer get( ) const {
			return m_ptr;
		}

		constexpr explicit operator bool( ) const {
			return static_cast<bool>( m_ptr );
		}

		constexpr reference operator*( ) const {
			return *m_ptr;
		}

		constexpr reference value( ) const {
			return *m_ptr;
		}

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr operator pointer( ) const {
			return m_ptr;
		}

		constexpr bool operator==( observer_ptr const &other ) const = default;
		// clang-format off
		constexpr std::strong_ordering operator<=>( observer_ptr const &other ) const = default;
		// clang-format on

		template<typename F>
		constexpr auto and_then( F &&f ) const {
			return monadic_ptr::and_then( *this, DAW_FWD( f ) );
		}

		template<typename F>
		constexpr auto or_else( F &&f ) const {
			return monadic_ptr::or_else( *this, DAW_FWD( f ) );
		}
	};

	template<typename P>
	observer_ptr( P * ) -> observer_ptr<P>;
} // namespace daw

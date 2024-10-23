// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"
#include "daw_use_default.h"

#include <memory>
#include <type_traits>

namespace daw {
	template<typename T, typename Owner = daw::use_default>
	struct read_only {
		static_assert( not std::is_reference_v<T>,
		               "Reference types are not supported" );
		static_assert( not std::is_const_v<T>, "Const types are not supported" );
		using value_type = T;

		friend Owner;

	private:
		value_type value{ };

		read_only &operator=( read_only const & ) = default;
		read_only &operator=( read_only && ) = default;

	public:
		read_only( ) = default;
		~read_only( ) = default;
		read_only( read_only const & ) = default;
		read_only( read_only && ) = default;

		constexpr read_only( T const &v ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> )
		  : value( v ) {}

		constexpr read_only( T &&v ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : value( std::move( v ) ) {}

		[[nodiscard]] constexpr operator value_type const &( ) const & noexcept {
			return value;
		}

		[[nodiscard]] constexpr operator value_type const &&( ) const && noexcept {
			return std::move( value );
		}

		[[nodiscard]] constexpr value_type const &get( ) const & noexcept {
			return value;
		}

		[[nodiscard]] constexpr value_type const &&get( ) const && noexcept {
			return std::move( value );
		}
	};
} // namespace daw

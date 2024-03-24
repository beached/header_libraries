// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"

#include <iterator>
#include <limits>

namespace daw {

	template<typename T>
	struct integer_range_iterator {
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = value_type const *;
		using reference = value_type const &;
		using iterator_category = std::input_iterator_tag;

		value_type current_value = 0;

		integer_range_iterator( ) = default;

		constexpr integer_range_iterator( value_type start_value ) noexcept
		  : current_value{ start_value } {}

		constexpr reference operator*( ) const noexcept {
			return current_value;
		}

		constexpr integer_range_iterator &operator++( ) noexcept {
			++current_value;
			return *this;
		}

		constexpr integer_range_iterator operator++( int ) const noexcept {
			integer_range_iterator tmp{ *this };
			++current_value;
			return tmp;
		}

		friend constexpr bool
		operator==( integer_range_iterator const &lhs,
		            integer_range_iterator const &rhs ) noexcept {
			return lhs.current_value == rhs.current_value;
		}

		friend constexpr bool
		operator!=( integer_range_iterator const &lhs,
		            integer_range_iterator const &rhs ) noexcept {
			return lhs.current_value != rhs.current_value;
		}
	};

	template<typename T>
	struct integer_range {
		integer_range_iterator<T> first;
		integer_range_iterator<T> last;

		constexpr integer_range_iterator<T> begin( ) noexcept {
			return first;
		}

		constexpr integer_range_iterator<T> end( ) noexcept {
			return last;
		}

		constexpr integer_range( T f, T l ) noexcept
		  : first{ f }
		  , last{ l } {}
	};
} // namespace daw

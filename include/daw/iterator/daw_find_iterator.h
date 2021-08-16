// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>
#include <iterator>
#include <limits>

namespace daw {

	template<typename Iterator1, typename Iterator2>
	struct find_iterator {
		using value_type = Iterator1;
		using difference_type = ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;
		using reference = value_type &;

		value_type current_value;

		constexpr find_iterator( ) noexcept
		  : current_value{ (std::numeric_limits<value_type>::max)( ) } {}
		constexpr find_iterator( value_type start_value ) noexcept
		  : current_value{ start_value } {}
		constexpr find_iterator( value_type start_value,
		                         value_type last_value ) noexcept
		  : current_value{ start_value } {}
		~find_iterator( ) noexcept = default;

		constexpr find_iterator( find_iterator const & ) noexcept = default;
		constexpr find_iterator &
		operator=( find_iterator const & ) noexcept = default;
		constexpr find_iterator( find_iterator && ) noexcept = default;
		constexpr find_iterator &operator=( find_iterator && ) noexcept = default;

		constexpr reference operator*( ) const noexcept {
			return current_value;
		}

		constexpr find_iterator &operator++( ) noexcept {
			++current_value;
			return *this;
		}

		constexpr find_iterator operator++( int ) const noexcept {
			find_iterator tmp{ *this };
			++current_value;
			return tmp;
		}

		friend constexpr bool operator==( find_iterator const &lhs,
		                                  find_iterator const &rhs ) noexcept {
			return lhs.current_value == rhs.current_value;
		}

		friend constexpr bool operator!=( find_iterator const &lhs,
		                                  find_iterator const &rhs ) noexcept {
			return lhs.current_value != rhs.current_value;
		}
	};
} // namespace daw

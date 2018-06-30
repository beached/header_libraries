// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

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

		value_type current_value;

		constexpr integer_range_iterator( ) noexcept
		  : current_value{std::numeric_limits<T>::max( )} {}
		constexpr integer_range_iterator( value_type start_value ) noexcept
		  : current_value{start_value} {}
		constexpr integer_range_iterator( value_type start_value,
		                                  value_type last_value ) noexcept
		  : current_value{start_value} {}
		~integer_range_iterator( ) noexcept = default;

		constexpr integer_range_iterator(
		  integer_range_iterator const & ) noexcept = default;
		constexpr integer_range_iterator &
		operator=( integer_range_iterator const & ) noexcept = default;
		constexpr integer_range_iterator( integer_range_iterator && ) noexcept =
		  default;
		constexpr integer_range_iterator &
		operator=( integer_range_iterator && ) noexcept = default;

		constexpr reference operator*( ) const noexcept {
			return current_value;
		}

		constexpr integer_range_iterator &operator++( ) noexcept {
			++current_value;
			return *this;
		}

		constexpr integer_range_iterator operator++( int ) const noexcept {
			integer_range_iterator tmp{*this};
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
		  : first{f}
		  , last{l} {}
	};
} // namespace daw

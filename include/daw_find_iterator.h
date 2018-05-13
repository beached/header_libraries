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

	template<typename Iterator1, typename Iterator2>
	struct find_iterator {
		using value_type = Iterator1;
		using difference_type = ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		value_type current_value;

		constexpr find_iterator( ) noexcept
		  : current_value{std::numeric_limits<T>::max( )} {}
		constexpr find_iterator( value_type start_value ) noexcept
		  : current_value{start_value} {}
		constexpr find_iterator( value_type start_value,
		                         value_type last_value ) noexcept
		  : current_value{start_value} {}
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
			find_iterator tmp{*this};
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

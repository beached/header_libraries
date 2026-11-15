// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"

#include <concepts>
#include <type_traits>

namespace daw {
	namespace recursive_equality_comparable_details {
		template<typename T, bool = Range<T>>
		struct is_recursively_equality_comparable
		  : std::bool_constant<std::equality_comparable<T>> {};

		template<typename T>
		struct is_recursively_equality_comparable<T, true>
		  : std::bool_constant<
		      std::equality_comparable<T> and
		      is_recursively_equality_comparable<range_const_reference_t<T>>::value> {
		};
	} // namespace recursive_equality_comparable_details

	template<typename T>
	struct is_recursively_equality_comparable
	  : recursive_equality_comparable_details::
	      is_recursively_equality_comparable<T> {};

	template<typename T>
	inline constexpr bool is_recursively_equality_comparable_v =
	  is_recursively_equality_comparable<T>::value;
} // namespace daw

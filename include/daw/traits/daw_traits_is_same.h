// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/stdinc/integral_constant.h>

namespace daw {
	template<typename T, typename U>
	inline constexpr bool is_same_v = false;

	template<typename T>
	inline constexpr bool is_same_v<T, T> = true;

	template<typename T, typename U>
	using is_same = std::bool_constant<is_same_v<T, U>>;

	template<typename T, typename U>
	using is_same_t = typename std::bool_constant<is_same_v<T, U>>::type;
} // namespace daw

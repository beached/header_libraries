// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <tuple>
#include <type_traits>

namespace daw {
	template<typename>
	inline constexpr bool is_tuple_v = false;

	template<typename... Args>
	inline constexpr bool is_tuple_v<std::tuple<Args...>> = true;

	template<typename T>
	using is_tuple = std::bool_constant<is_tuple_v<T>>;
} // namespace daw

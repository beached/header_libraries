// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <type_traits>

namespace daw::traits {
	template<typename T>
	concept IntegerEnum = std::is_integral_v<T> or std::is_enum_v<T>;
	// Get the underlying type or integral type passed
	template<IntegerEnum T>
	using integral_type_t =
	  typename std::conditional_t<std::is_enum_v<T>, std::underlying_type<T>,
	                              std::type_identity<T>>::type;

} // namespace daw::traits

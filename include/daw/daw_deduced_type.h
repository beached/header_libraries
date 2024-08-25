// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	// Sentinel type to mark type as being deduced
	struct deduced_type {};

	template<typename T>
	inline constexpr bool is_deduced_type_v = false;

	template<>
	inline constexpr bool is_deduced_type_v<deduced_type> = true;
} // namespace daw

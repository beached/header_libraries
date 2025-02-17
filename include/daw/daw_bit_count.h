// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <climits>
#include <cstddef>

namespace daw {
	template<typename T>
	inline constexpr std::size_t bit_count_v = sizeof( T ) * CHAR_BIT;
}

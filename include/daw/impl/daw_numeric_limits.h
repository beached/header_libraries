// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <limits>

namespace daw {
	template<typename T>
	struct numeric_limits : std::numeric_limits<T> {};
} // namespace daw

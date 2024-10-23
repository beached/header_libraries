// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	// Sentinel type to mark default type parameters
	struct use_default {
		explicit use_default( ) = default;
	};
} // namespace daw

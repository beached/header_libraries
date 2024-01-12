// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_typeof.h"

#include <daw/stdinc/integral_constant.h>

namespace daw {
	template<auto Value>
	using constant = std::integral_constant<DAW_TYPEOF( Value ), Value>;

}

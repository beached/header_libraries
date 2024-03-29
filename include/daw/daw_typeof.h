// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_remove_cvref.h"

#define DAW_TYPEOF( ... ) daw::remove_cvref_t<decltype( __VA_ARGS__ )>

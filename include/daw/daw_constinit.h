// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( __cpp_constinit )
#define DAW_CONSTINIT constinit
#define DAW_HAS_CONSTINIT
#else
#define DAW_CONSTINIT
#endif

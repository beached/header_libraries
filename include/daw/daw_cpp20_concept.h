// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#if defined( DAW_HAS_CPP20_CONCEPTS )
#define DAW_CPP20_CONCEPT concept
#define DAW_CPP20_TYPENAME_CONSTRAINT(...) __VA_ARGS__
#else
#define DAW_CPP20_CONCEPT inline constexpr bool
#define DAW_CPP20_TYPENAME_CONSTRAINT(...) typename
#endif


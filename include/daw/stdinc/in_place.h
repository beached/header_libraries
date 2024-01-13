// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if defined( DAW_HAS_STD_LIBCPP ) and __has_include( <__utility/in_place.h> )
#include <__utility/in_place.h>
#elif defined( DAW_HAS_STD_LIBSTDCPP ) and __has_include( <bits/utility.h> )
#include <bits/utility.h>
#else
#include <utility>
#endif

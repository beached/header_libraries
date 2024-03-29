// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

#if defined( DAW_HAS_STD_LIBCPP ) and __has_include( <__algorithm/min.h> ) \
and __has_include( <__algorithm/max.h> )
#include <__algorithm/max.h>
#include <__algorithm/min.h>
#elif defined( DAW_HAS_STD_LIBSTDCPP ) and __has_include( <bits/stl_algobase.h> )
#include <bits/stl_algobase.h>
#else
#include <algorithm>
#endif

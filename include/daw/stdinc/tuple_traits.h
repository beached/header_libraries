// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>

#if defined( __GLIBCXX__ ) and __has_include( <bits/utility.h> )
#include <bits/utility.h>
#elif defined( _LIBCPP_VERSION ) and __has_include( <__tuple/tuple_traits.h> )
#include <__tuple/tuple_traits.h>
#else
#include <tuple>
#endif

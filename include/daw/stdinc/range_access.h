// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>

#if defined( __GLIBCXX__ ) and __has_include( <bits/range_access.h> )
#include <bits/range_access.h>
#elif defined( _LIBCPP_VERSION ) and \
  __has_include( <__iterator/next.h> ) and __has_include( <__iterator/prev.h> )
#include <__iterator/next.h>
#include <__iterator/prev.h>
#else
#include <iterator>
#endif

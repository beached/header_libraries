// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

#if defined( DAW_HAS_STD_LIBCPP ) and \
  __has_include( <__iterator/reverse_iterator.h>)
#include <__iterator/reverse_iterator.h>
#elif defined( DAW_HAS_STD_LIBSTDCPP ) and __has_include( <bits/stl_iterator.h> )
#include <bits/stl_iterator.h>
#else
#include <iterator>
#endif

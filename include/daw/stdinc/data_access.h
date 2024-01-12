// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_cpp_feature_check.h>

#if DAW_HAS_CLANG_VER_LT( 10, 0 )
#include <string_view>
#else
#if defined( DAW_HAS_STD_LIBSTDCPP ) and __has_include( <bits/range_access.h> )
#include <bits/range_access.h>
#elif defined( DAW_HAS_STD_LIBCPP ) and \
  __has_include( <__iterator/data.h> ) and __has_include( <__iterator/size.h> )
#include <__iterator/data.h>
#include <__iterator/size.h>
#else
#include <string_view>
#endif
#endif

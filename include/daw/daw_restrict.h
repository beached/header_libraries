// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

/// @brief Allow restrict on supported compilers unless it is already defined.
#if not defined( DAW_RESTRICT ) and                                            \
  ( defined( _MSC_VER ) or defined( __clang__ ) or defined( __GNUC__ ) )
#define DAW_RESTRICT __restrict
#else
#define DAW_RESTRICT
#endif
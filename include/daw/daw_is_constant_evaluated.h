// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "ciso646.h"
#include "daw_cpp_feature_check.h"

#if defined( __cpp_lib_is_constant_evaluated )

#include <type_traits>

#define DAW_IS_CONSTANT_EVALUATED( ) std::is_constant_evaluated( )

#elif DAW_HAS_BUILTIN( __builtin_is_constant_evaluated )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )

#elif DAW_HAS_MSVC_VER_GTE( 1925 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )

#elif DAW_HAS_GCC_VER_GTE( 9, 1 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )

#elif DAW_HAS_CLANGCL_VER_GTE( 9, 0 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )

#endif

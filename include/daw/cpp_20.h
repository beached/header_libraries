// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( __cpp_constexpr_dynamic_alloc )
#if __cpp_constexpr_dynamic_alloc >= 201907L
#define DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC
#endif
#endif

#if defined( DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC )
#define DAW_CX_DTOR constexpr
#define DAW_CPP20_CX_DTOR constexpr
#define DAW_CPP20_CX_ALLOC constexpr
#else
#define DAW_CX_DTOR
#define DAW_CPP20_CX_DTOR
#define DAW_CPP20_CX_ALLOC
#endif
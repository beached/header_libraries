// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( __cpp_consteval ) 
	#define DAW_CONSTEVAL consteval
	#define DAW_HAS_CONSTEVAL
#else
	#define DAW_CONSTEVAL constexpr
#endif



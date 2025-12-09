// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#include <cstddef>
#include <type_traits>

#if defined( DAW_HAS_CPP20_CONCEPTS ) and not defined( DAW_NO_REQUIRES )
#define DAW_USE_REQUIRES
#define DAW_ENABLEIF( ... )
#define DAW_ENABLEIF2( ... )
#define DAW_ENABLEIF_S( ... )
#define DAW_REQUIRES( ... ) requires( __VA_ARGS__ )
#else
#define DAW_ENABLEIF( ... ) \
	, std::enable_if_t<( __VA_ARGS__ ), std::nullptr_t> = nullptr
#define DAW_ENABLEIF2( ... ) , std::enable_if_t<( __VA_ARGS__ ), std::nullptr_t>
#define DAW_ENABLEIF_S( ... ) , std::enable_if_t<( __VA_ARGS__ )>
#define DAW_REQUIRES( ... )
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#if DAW_HAS_BUILTIN( __builtin_expect )

#define DAW_LIKELY( ... ) ( __builtin_expect( !!( __VA_ARGS__ ), 1 ) )
#define DAW_UNLIKELY( ... ) ( __builtin_expect( !!( __VA_ARGS__ ), 0 ) )

#else

#define DAW_LIKELY( ... ) !!( __VA_ARGS__ )
#define DAW_UNLIKELY( ... ) !!( __VA_ARGS__ )

#endif
#if __has_cpp_attribute( likely )
#define DAW_LIKELY_BRANCH [[likely]]
#else
#define DAW_LIKELY_BRANCH [[]]
#endif

#if __has_cpp_attribute( unlikely )
#define DAW_UNLIKELY_BRANCH [[unlikely]]
#else
#define DAW_UNLIKELY_BRANCH [[]]
#endif

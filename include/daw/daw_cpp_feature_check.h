// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <ciso646>

#if defined( __has_include )
#define DAW_HAS_INCLUDE( ... ) (__has_include( __VA_ARGS__ ))
#if __has_include( <version> )
#include <version>
#endif
#else
#define DAW_HAS_INCLUDE( ... ) ( false )
#endif

#if defined( __has_builtin )
#define DAW_HAS_BUILTIN( ... ) __has_builtin( __VA_ARGS__ )
#else
#define DAW_HAS_BUILTIN( ... ) ( false )
#endif

// Do not use when -Wundef is at play as -Werror will make it not build
#define DAW_HAS_FEATURE( feature ) ( ( feature ) > 0 )

// Do not use when -Wundef is at play as -Werror will make it not build
#define DAW_HAS_FEATURE_VERSION( feature, version )                            \
	( ( feature ) > ( version ) )

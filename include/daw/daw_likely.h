// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_LIKELY( ... ) ( __builtin_expect( !!( __VA_ARGS__ ), 1 ) )
#define DAW_UNLIKELY( ... ) ( __builtin_expect( !!( __VA_ARGS__ ), 0 ) )
#elif defined( _MSC_VER )
#define DAW_LIKELY( ... ) !!( __VA_ARGS__ )
#define DAW_UNLIKELY( ... ) !!( __VA_ARGS__ )
#else
#define DAW_LIKELY( ... ) !!( __VA_ARGS__ )
#define DAW_UNLIKELY( ... ) !!( __VA_ARGS__ )
#endif

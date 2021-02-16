// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>

#ifndef NDEBUG
#define DAW_ASSUME( ... ) assert( __VA_ARGS__ )
#else
#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_ASSUME( ... )                                                      \
	while( not( ( __VA_ARGS__ ) ) ) {                                            \
		__builtin_unreachable( );                                                  \
	}                                                                            \
	do {                                                                         \
	} while( false )
#elif defined( _MSC_VER )
#define DAW_ASSUME( ... ) __assume( !!( ( __VA_ARGS__ ) ) )
#else
#define DAW_ASSUME( ... )                                                      \
	do {                                                                         \
	} while( false )
#endif
#endif

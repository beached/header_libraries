// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_ASSUME( expr )    \
	while( !( expr ) ) {        \
		__builtin_unreachable( ); \
	}                           \
	do {                        \
	} while( false )
#elif defined( _MSC_VER )
#define DAW_ASSUME( expr ) __assume( !!(expr) )
#else
#define DAW_ASSUME( expr ) do { } while( false )
#endif

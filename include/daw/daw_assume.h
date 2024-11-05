// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_unreachable.h"

#if defined( DEBUG )
#include <cassert>
#define DAW_ASSUME( ... )          \
	do {                             \
		if( not( ( __VA_ARGS__ ) ) ) { \
			assert( ( __VA_ARGS__ ) );   \
			DAW_UNREACHABLE( );          \
		}                              \
	} while( false )
#else
#define DAW_ASSUME( ... )          \
	do {                             \
		if( not( ( __VA_ARGS__ ) ) ) { \
			DAW_UNREACHABLE( );          \
		}                              \
	} while( false )
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#include <cassert>

#if defined( DEBUG )

#define DAW_ASSUME( ... ) assert( ( __VA_ARGS__ ) )

#elif DAW_HAS_BUILTIN( __builtin_assume )

#define DAW_ASSUME( ... ) __builtin_assume( ( __VA_ARGS__ ) )

#elif DAW_HAS_BUILTIN( __builtin_unreachable )

#define DAW_ASSUME( ... )                                                      \
	do {                                                                         \
		if( not( ( __VA_ARGS__ ) ) ) {                                             \
			__builtin_unreachable( );                                                \
		}                                                                          \
	} while( false )

#elif defined( _MSC_VER )

#define DAW_ASSUME( ... ) __assume( !!( ( __VA_ARGS__ ) ) )

#else

#include <exception>
#define DAW_ASSUME( ... )                                                      \
	do {                                                                         \
		if( not( ( __VA_ARGS__ ) ) ) {                                             \
			std::terminate( );                                                       \
		}                                                                          \
	} while( false )

#endif

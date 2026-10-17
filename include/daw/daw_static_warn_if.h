// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_cpp_feature_check.h"

#if DAW_HAS_GCC_VER_GTE( 9, 0 ) or DAW_HAS_CLANG_VER_GTE( 14, 0 )

#define daw_static_warn_if( Bool, Message )                      \
	do {                                                           \
		_Pragma( "GCC diagnostic push" );                            \
		_Pragma( "GCC diagnostic warning \"-Wattribute-warning\"" ); \
		if( __builtin_constant_p( Bool ) and Bool ) {                \
			[]( ) __attribute__( ( warning( Message ) ) ) {}( );       \
		}                                                            \
		_Pragma( "GCC diagnostic pop" )                              \
	} while( false )
#else
#define daw_static_warn_if( Bool, Message ) \
	do {                                      \
	} while( false )
#endif

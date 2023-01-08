// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

// small compiler specific fixups

#if DAW_HAS_CLANG_VER_GTE( 16, 0 )
#define DAW_UNSAFE_BUFFER_FUNC_START  \
	_Pragma( "clang diagnostic push" ); \
	_Pragma( R"(clang diagnostic ignored "-Wunsafe-buffer-usage")" )
#define DAW_UNSAFE_BUFFER_FUNC_STOP _Pragma( "clang diagnostic pop" )
#else
#define DAW_UNSAFE_BUFFER_FUNC_START
#define DAW_UNSAFE_BUFFER_FUNC_STOP
#endif



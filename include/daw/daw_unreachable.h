// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_UNREACHABLE( ) ( __builtin_unreachable( ) )
#elif defined( _MSC_VER )
#define DAW_UNREACHABLE( ) __assume( false );
#else
#include <exception>
#define DAW_UNREACHABLE( ) std::terminate( )
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#if __has_include( <version> )
#include <version>
#endif

#if defined( __cpp_lib_is_constant_evaluated )

#include <type_traits>
#define DAW_IS_CONSTANT_EVALUATED( ) std::is_constant_evaluated( )
#elif defined( __has_builtin )

#if __has_builtin( __builtin_is_constant_evaluated )
#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( ) 
#endif

#endif

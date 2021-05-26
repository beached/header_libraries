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
#define DAW_HAS_INCLUDE( Include ) __has_include( Include )
#if DAW_HAS_INCLUDE( <version> )
#include <version>
#endif
#else
#define DAW_HAS_INCLUDE( Include ) false
#endif

#if defined( __has_builtin )
#define DAW_HAS_BUILTIN( Name ) __has_builtin( Name )
#else
#define DAW_HAS_BUILTIN( Name ) false
#endif


// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// This file allows older MSVC, not in C++ 20 mode, to use alternative tokens
// while in permissive mode

#pragma once

#if defined( _MSC_VER )
#if _MSC_VER < 1930 or _MSVC_LANG < 202002L
#include <ciso646>
#endif
#endif
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

#if defined( _MSC_EXTENSIONS ) || defined( DAW_FORCE_CISO646 ) || \
  defined( _MSC_VER )
#if defined( _MSC_EXTENSIONS ) || defined( DAW_FORCE_CISO646 ) || \
  _MSC_VER < 1930 || _MSVC_LANG < 202002L
#define and &&
#define and_eq &=
#define bitand &
#define bitor |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^
#define xor_eq ^=
#endif
#endif

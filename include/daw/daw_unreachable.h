// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( DAW_HAS_CPP23_UNREACHABLE )
#include <utility>
#define DAW_UNREACHABLE_IMPL( ) std::unreachable( )
#elif DAW_HAS_BUILTIN( __builtin_unreachable )
#define DAW_UNREACHABLE_IMPL( ) __builtin_unreachable( )
#elif defined( DAW_HAS_MSVC )
#define DAW_UNREACHABLE_IMPL( ) __assume( false );
#elif defined( DAW_HAS_GCC_LIKE )
// All gcc/clang compilers supporting c++17 have __builtin_unreachable
#define DAW_UNREACHABLE_IMPL( ) __builtin_unreachable( )
#else
#include <exception>
#define DAW_UNREACHABLE_IMPL( ) std::terminate( )
#endif

#if defined( DEBUG )
#include <exception>
#define DAW_UNREACHABLE( ) \
  std::terminate( ); \
  DAW_UNREACHABLE_IMPL( )
#else
#define DAW_UNREACHABLE( ) DAW_UNREACHABLE_IMPL( )
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_cpp_feature_check.h"

#if DAW_HAS_BUILTIN( __builtin_is_constant_evaluated )
#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )
#define DAW_HAS_IS_CONSTANT_EVALUATED 1

#elif DAW_HAS_MSVC_VER_GTE( 1925 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )
#define DAW_HAS_IS_CONSTANT_EVALUATED 1

#elif DAW_HAS_GCC_VER_GTE( 9, 1 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )
#define DAW_HAS_IS_CONSTANT_EVALUATED 1

#elif DAW_HAS_CLANGCL_VER_GTE( 9, 0 )

#define DAW_IS_CONSTANT_EVALUATED( ) __builtin_is_constant_evaluated( )
#define DAW_HAS_IS_CONSTANT_EVALUATED 1

#elif defined( __cpp_lib_is_constant_evaluated )
#error "Hi2"
#include <type_traits>
#define DAW_IS_CONSTANT_EVALUATED( ) std::is_constant_evaluated( )
#define DAW_HAS_IS_CONSTANT_EVALUATED 1

#endif

#if defined( DAW_HAS_IS_CONSTANT_EVALUATED )
#define DAW_IS_CONSTANT_EVALUATED_COMPAT( ) DAW_IS_CONSTANT_EVALUATED( )
#else
#define DAW_IS_CONSTANT_EVALUATED_COMPAT( ) true
#endif

#if defined( __cpp_if_consteval )
#if __cpp_if_consteval >= 202106L
#define DAW_HAS_IF_CONSTEVAL
#endif
#endif

#if defined( DAW_HAS_IF_CONSTEVAL )
#define DAW_IF_CONSTEVAL if consteval
#define DAW_IF_NOT_CONSTEVAL if not consteval
#define DAW_HAS_IF_CONSTEVAL_COMPAT
#elif defined( DAW_HAS_IS_CONSTANT_EVALUATED )
#define DAW_IF_CONSTEVAL if( DAW_IS_CONSTANT_EVALUATED( ) )
#define DAW_IF_NOT_CONSTEVAL if( not DAW_IS_CONSTANT_EVALUATED( ) )
#define DAW_HAS_IF_CONSTEVAL_COMPAT
#endif

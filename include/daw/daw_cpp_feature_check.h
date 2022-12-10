// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>

#if defined( __has_include )
#define DAW_HAS_INCLUDE( ... ) (__has_include( __VA_ARGS__ ))
#if __has_include( <version> )
#include <version>
#endif
#else
#define DAW_HAS_INCLUDE( ... ) ( false )
#endif

#if defined( __clang__ )
#define DAW_HAS_CLANG_VER( ver_maj, ver_min ) \
	( ( __clang_major__ > ( ver_maj ) ) or      \
	  ( ( __clang_major__ == ( ver_maj ) ) and  \
	    ( __clang_minor__ >= ( ver_min ) ) ) )
#define DAW_HAS_CLANG
#else
#define DAW_HAS_CLANG_VER( ver_maj, ver_min ) ( false )
#endif

#if defined( __GNUC__ ) and not defined( __clang__ )
#define DAW_HAS_GCC_VER( ver_maj, ver_min ) \
	( ( ( __GNUC__ > ( ver_maj ) ) ) or       \
	  ( ( __GNUC__ == ( ver_maj ) ) and ( __GNUC_MINOR__ >= ( ver_min ) ) ) )
#define DAW_HAS_GCC
#else
#define DAW_HAS_GCC_VER( ver_maj, ver_min ) ( false )
#endif

#if defined( __MSC_VER ) and not defined( __clang__ )
#define DAW_HAS_MSVC_VER( ver ) ( __MSC_VER >= ( ver ) )
#define DAW_HAS_MSVC
#else
#define DAW_HAS_MSVC_VER( ver ) ( false )
#endif

#if defined( __has_builtin )
#define DAW_HAS_BUILTIN( name ) __has_builtin( name )
#else
#define DAW_HAS_BUILTIN( name ) ( false )
#endif

// Do not use when -Wundef is at play as -Werror will make it not build
#define DAW_HAS_FEATURE( feature ) ( ( feature ) > 0 )

// Do not use when -Wundef is at play as -Werror will make it not build
#define DAW_HAS_FEATURE_VERSION( feature, version ) \
	( ( feature ) > ( version ) )

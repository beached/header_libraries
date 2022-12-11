// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"

#if defined( __has_include )
#define DAW_HAS_INCLUDE( ... ) (__has_include( __VA_ARGS__ ))
#if __has_include( <version> )
#include <version>
#endif
#else
#define DAW_HAS_INCLUDE( ... ) ( false )
#endif

#if defined( __clang__ )
// Check if clang has a version >= to that specified
#define DAW_HAS_CLANG_VER_GTE( ver_maj, ver_min ) \
	( ( __clang_major__ > ( ver_maj ) ) or          \
	  ( ( __clang_major__ == ( ver_maj ) ) and      \
	    ( __clang_minor__ >= ( ver_min ) ) ) )

// Check if clang has a version < to that specified
#define DAW_HAS_CLANG_VER_LT( ver_maj, ver_min ) \
	( ( __clang_major__ < ( ver_maj ) ) or         \
	  ( ( __clang_major__ == ( ver_maj ) ) and     \
	    ( __clang_minor__ < ( ver_min ) ) ) )

#define DAW_HAS_CLANG
#define DAW_HAS_GCC_LIKE
#else
#define DAW_HAS_CLANG_VER_GTE( ver_maj, ver_min ) ( false )
#define DAW_HAS_CLANG_VER_LT( ver_maj, ver_min ) ( false )
#endif

#if defined( __GNUC__ ) and not defined( __clang__ )
// Check if gcc has a version >= to that specified
#define DAW_HAS_GCC_VER_GTE( ver_maj, ver_min ) \
	( ( ( __GNUC__ > ( ver_maj ) ) ) or           \
	  ( ( __GNUC__ == ( ver_maj ) ) and ( __GNUC_MINOR__ >= ( ver_min ) ) ) )

// Check if gcc has a version < to that specified
#define DAW_HAS_GCC_VER_LT( ver_maj, ver_min ) \
	( ( __GNUC__ < ( ver_maj ) ) or              \
	  ( ( __GNUC__ == ( ver_maj ) ) and ( __GNUC_MINOR__ < ( ver_min ) ) ) )

#define DAW_HAS_GCC
#define DAW_HAS_GCC_LIKE
#else
#define DAW_HAS_GCC_VER_GTE( ver_maj, ver_min ) ( false )
#define DAW_HAS_GCC_VER_LT( ver_maj, ver_min ) ( false )
#endif

#if defined( _MSC_VER ) and not defined( __clang__ )
// Check if MSVC has a version >= to that specified
#define DAW_HAS_MSVC_VER_GTE( ver ) ( _MSC_VER >= ( ver ) )

// Check if MSVC has a version < to that specified
#define DAW_HAS_MSVC_VER_LT( ver ) ( _MSC_VER < ( ver ) )
#define DAW_HAS_MSVC
#define DAW_HAS_MSVC_LIKE
#else
#define DAW_HAS_MSVC_VER_GTE( ver ) ( false )
#define DAW_HAS_MSVC_VER_LT( ver ) ( false )
#endif

#if defined( _MSC_VER ) and defined( __clang__ )
// Check if clangcl has a version >= to that specified
#define DAW_HAS_CLANGCL_VER_GTE( ver_maj, ver_min ) \
	( ( __clang_major__ > ( ver_maj ) ) or            \
	  ( ( __clang_major__ == ( ver_maj ) ) and        \
	    ( __clang_minor__ >= ( ver_min ) ) ) )

// Check if clangcl has a version < to that specified
#define DAW_HAS_CLANGCL_VER_LT( ver_maj, ver_min ) \
	( ( __clang_major__ < ( ver_maj ) ) or           \
	  ( ( __clang_major__ == ( ver_maj ) ) and       \
	    ( __clang_minor__ < ( ver_min ) ) ) )

#define DAW_HAS_CLANGCL
#define DAW_HAS_CLANG
#define DAW_HAS_MSVC_LIKE
#else
#define DAW_HAS_CLANGCL_VER_GTE( ver_maj, ver_min ) ( false )
#define DAW_HAS_CLANGCL_VER_LT( ver_maj, ver_min ) ( false )
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

#if defined( DAW_HAS_MSVC )
#define DAW_CPP_VERSION _MSVC_LANG
#else
#define DAW_CPP_VERSION __cplusplus
#endif
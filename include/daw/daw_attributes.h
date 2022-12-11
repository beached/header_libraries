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

#if defined( DAW_HAS_GCC_LIKE )

#if not defined( DAW_NO_FLATTEN )

#define DAW_ATTRIB_FLATTEN [[gnu::flatten]]
#define DAW_ATTRIB_FLATINLINE [[gnu::flatten, gnu::always_inline]]

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE

#endif

#ifndef DAW_FORCE_ALL_HIDDEN
#define DAW_ATTRIB_PUBLIC __attribute__( ( visibility( "default" ) ) )
#else
#define DAW_ATTRIB_PUBLIC __attribute__( ( visibility( "hidden" ) ) )
#endif

#define DAW_ATTRIB_HIDDEN __attribute__( ( visibility( "hidden" ) ) )

#if not defined( DAW_NO_FORCED_INLINE )
#define DAW_ATTRIB_INLINE [[gnu::always_inline]] inline
#else
#define DAW_ATTRIB_INLINE inline
#endif
#define DAW_ATTRIB_NOINLINE [[gnu::noinline]]

#elif defined( DAW_HAS_MSVC )

#if not defined( DAW_NO_FLATTEN )

#ifdef DAW_ATTRIB_USE_NEW_MSVC
#define DAW_ATTRIB_FLATTEN [[msvc::flatten]]
#define DAW_ATTRIB_FLATINLINE [[msvc::flatten, msvc::inline]]
#else
#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE
#endif

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE

#endif

#ifndef DAW_FORCE_ALL_HIDDEN
#define DAW_ATTRIB_PUBLIC __declspec( dllexport )
#else
#define DAW_ATTRIB_PUBLIC
#endif
#define DAW_ATTRIB_HIDDEN

#if not defined( DAW_NO_FORCED_INLINE )
#ifdef DAW_ATTRIB_USE_NEW_MSVC
#define DAW_ATTRIB_INLINE [[msvc::forceinline]] inline
#else
#define DAW_ATTRIB_INLINE __forceinline
#endif
#else
#define DAW_ATTRIB_INLINE inline
#endif
#define DAW_ATTRIB_NOINLINE __declspec( noinline )

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE
#define DAW_ATTRIB_HIDDEN
#define DAW_ATTRIB_PUBLIC
#define DAW_ATTRIB_INLINE inline
#define DAW_ATTRIB_NOINLINE

#endif

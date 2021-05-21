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

#if not defined( DAW_NO_FLATTEN )

#define DAW_ATTRIB_FLATTEN [[gnu::flatten]]
#define DAW_ATTRIB_FLATINLINE [[gnu::flatten, gnu::always_inline]]

#else

#define DAW_ATTRIB_FLATTEN 
#define DAW_ATTRIB_FLATINLINE

#endif

#define DAW_ATTRIB_HIDDEN __attribute__( ( visibility( "hidden" ) ) )
#define DAW_ATTRIB_INLINE [[gnu::always_inline]]
#define DAW_ATTRIB_NOINLINE [[gnu::noinline]]

#elif defined( _MSC_VER )

#if not defined( DAW_NO_FLATTEN )

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE __forceinline

#else

#define DAW_ATTRIB_FLATTEN 
#define DAW_ATTRIB_FLATINLINE

#endif

#define DAW_ATTRIB_HIDDEN
#define DAW_ATTRIB_INLINE __forceinline
#define DAW_ATTRIB_NOINLINE __declspec( noinline )

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE
#define DAW_ATTRIB_HIDDEN
#define DAW_ATTRIB_INLINE
#define DAW_ATTRIB_NOINLINE

#endif

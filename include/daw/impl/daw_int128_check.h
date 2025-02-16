// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if defined( DAW_JSON_NO_INT128 )
#define DAW_NO_INT128
#if defined( DAW_HAS_INT128 )
#undef DAW_HAS_INT128
#endif
#elif defined( DAW_HAS_INT128 )
#elif defined( __SIZEOF_INT128__ )
#if ( defined( __clang__ ) and not defined( _WIN32 ) ) or    \
  ( defined( __CUDACC__ ) and __CUDACC_VER_MAJOR__ >= 9 ) or \
  ( defined( __GNUC__ ) and not defined( __clang__ ) and     \
    not defined( __CUDACC__ ) )
#define DAW_HAS_INT128
#elif defined( __CUDACC__ )
// __CUDACC_VER__ is a full version number before CUDA 9, and is defined to a
// string explaining that it has been removed starting with CUDA 9. We use
// nested #ifs because there is no short-circuiting in the preprocessor.
// NOTE: `__CUDACC__` could be undefined while `__CUDACC_VER__` is defined.
#if __CUDACC_VER__ >= 70000
#define DAW_HAS_INT128
#endif
#endif
#endif

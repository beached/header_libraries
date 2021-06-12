// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if !defined( DAW_NO_FLATTEN ) && !defined( _MSC_VER )
#define DAW_ATTRIBUTE_FLATTEN [[gnu::flatten, gnu::always_inline]]
#define DAW_ATTRIBUTE_HIDDEN __attribute__( ( visibility( "hidden" ) ) )
#else
#define DAW_ATTRIBUTE_FLATTEN
#define DAW_ATTRIBUTE_HIDDEN
#endif

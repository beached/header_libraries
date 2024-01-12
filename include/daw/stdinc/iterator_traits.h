// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>

#if defined( _LIBCPP_VERSION ) and __has_include( <__iterator/iterator_traits.h> )
#include <__iterator/iterator_traits.h>
#else
#include <iterator>
#endif

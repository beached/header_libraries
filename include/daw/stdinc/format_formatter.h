// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#if DAW_CPP_VERSION < 202002L
#error "Formatters require at least C++ 20"
#endif

#if defined( DAW_HAS_STD_LIBCPP ) and __has_include( <__format/formatter.h> )
#include <__format/formatter.h>
#elif defined( DAW_HAS_STD_LIBSTDCPP ) and __has_include( <bits/formatfwd.h> )
#include <bits/formatfwd.h>
#elif defined( _MSC_VER ) and __has_include( <__msvc_formatter.hpp> )
#include <__msvc_formatter.hpp>
#else
#include <format>
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/impl/daw_int128_check.h"

#if not defined( DAW_NO_INT128 )
#if DAW_CPP_VERSION >= 202002L and DAW_HAS_MSVC_VER_GTE( 1932 )
// Earlier versions only supported C++20
#define DAW_HAS_MSVC_INT128
#elif DAW_HAS_MSVC_VER_GTE( 1934 )
#define DAW_HAS_MSVC_INT128
#endif
#endif

#if defined( DAW_HAS_MSVC_INT128 )
#define DAW_HAS_INT128
#include <__msvc_int128.hpp>

namespace daw {
	using int128_t = std::_Signed128;
	using uint128_t = std::_Unsigned128;
} // namespace daw
#endif

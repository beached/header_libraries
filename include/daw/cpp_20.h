// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#include <type_traits>

#if defined( __cpp_constexpr_dynamic_alloc )
#if __cpp_constexpr_dynamic_alloc >= 201907L
#define DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC
#endif
#endif

#if defined( DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC )
#define DAW_CX_DTOR constexpr
#define DAW_CPP20_CX_DTOR constexpr
#define DAW_CPP20_CX_ALLOC constexpr
#else
#define DAW_CX_DTOR
#define DAW_CPP20_CX_DTOR
#define DAW_CPP20_CX_ALLOC
#endif

#if defined( __cpp_lib_three_way_comparison )
#if __cpp_lib_three_way_comparison >= 201907L
#if __has_include( <compare> )
#include <compare>
#define DAW_HAS_CPP20_3WAY_COMPARE
#endif
#endif
#endif

namespace daw {
	template<typename T>
	inline constexpr bool is_unbounded_array_v = false;

	template<typename T>
	inline constexpr bool is_unbounded_array_v<T[]> = true;

	template<typename T>
	struct is_unbounded_array : std::bool_constant<is_unbounded_array_v<T>> {};
} // namespace daw
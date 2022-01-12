// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( __cpp_consteval )
#if __cpp_consteval >= 201811L
#define DAW_HAS_CONSTEVAL
#endif
#endif

#if defined( DAW_HAS_CONSTEVAL )
#define DAW_CONSTEVAL consteval
namespace daw {
	// Force evaluation of v at compile time
	template<typename T>
	consteval T const_evaluate( T &&v ) {
		return v;
	}
} // namespace daw
#define DAW_CONST_EVALUATE( ... ) ::daw::const_evaluate( __VA_ARGS__ )
#else
#define DAW_CONSTEVAL constexpr
// Does not guarantee evaluation, but should ensure it is checked for being a constant expression
#define DAW_CONST_EVALUATE( ... ) [&]( ) constexpr { return __VA_ARGS__; }( )
#endif

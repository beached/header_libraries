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
#define DAW_CONSTEVAL consteval
#define DAW_HAS_CONSTEVAL
#include "daw_move.h"
namespace daw {
	// Force evaluation of v at compile time
	template<typename T>
	consteval T const_evaluate( T &&v ) {
		return DAW_FWD2( T, v );
	}
} // namespace daw
#define DAW_CONST_EVALUATE( ... ) const_evaluate( __VA_ARGS__ )
#else
#define DAW_CONSTEVAL constexpr
#define DAW_CONST_EVALUATE( ... ) ( __VA_ARGS__ )
#endif

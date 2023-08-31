// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_cpp_feature_check.h"

#if defined( _MSC_VER )
#if _MSC_VER < 1930 and not defined( DAW_NO_CONSTEVAL )
#define DAW_NO_CONSTEVAL
#endif
#endif

#if defined( __cpp_consteval ) and not defined( DAW_NO_CONSTEVAL )
#if __cpp_consteval >= 201811L and not defined( DAW_HAS_CONSTEVAL )
#define DAW_HAS_CONSTEVAL
#endif
#endif

#if __cplusplus >= 202002 and defined( __clang_major__ ) and \
  not defined( DAW_NO_CONSTEVAL )
#if __clang_major__ >= 15 and not defined( DAW_HAS_CONSTEVAL )
#define DAW_HAS_CONSTEVAL
#endif
#endif

#if defined( DAW_HAS_CONSTEVAL )
#define DAW_CONSTEVAL consteval
namespace daw {
	// Force evaluation of v at compile time
	template<typename T>
	consteval auto const_evaluate( T &&v ) {
		return static_cast<T &&>( v );
	}
} // namespace daw
#define DAW_CONST_EVALUATE( ... ) ::daw::const_evaluate( __VA_ARGS__ )
#define DAW_AS_CONSTANT( ... ) ::daw::const_evaluate( __VA_ARGS__ )
#else
#define DAW_CONSTEVAL constexpr
// Cannot do, fallback to expression
#define DAW_CONST_EVALUATE( ... ) ( __VA_ARGS__ )
#define DAW_AS_CONSTANT( ... ) ( __VA_ARGS__ )
#endif

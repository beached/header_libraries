// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_ensure.h>
#include <daw/daw_formatters.h>

#include <format>

int main( ) {
#if DAW_HAS_CLANG_VER_LT( 15, 1 ) and defined( __APPLE_CC__ )
	// Apple clang fails because of formatters
	constexpr std::string_view sv = "Hello World!";
#else
	constexpr daw::string_view sv = "Hello World!";
#endif
	auto const s = std::format( "{}", sv );
	daw_ensure( s == sv );
}

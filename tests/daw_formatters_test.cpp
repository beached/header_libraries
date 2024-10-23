// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_formatters.h>

#include <format>

int main( ) {
	constexpr daw::string_view sv = "Hello World!";
	auto const s = std::format( "{}", sv );
	daw_ensure( s == sv );
}

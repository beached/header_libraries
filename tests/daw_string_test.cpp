// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_string.h"

#include <cctype>
#include <string>
#include <vector>

void make_string_testing( ) {
	std::string a = "a b c d e f";
	auto b = daw::string::split_if( a, &isspace );

	daw::expecting( 6U, b.size( ) );
}

void string_fmt_test_001( ) {
	auto result = daw::string::fmt( "This is {0} test", "a" );
	daw::expecting( "This is a test", result );
}

int main( ) {
	make_string_testing( );
	string_fmt_test_001( );
}

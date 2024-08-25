// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/InputIterator.h"

#include "daw/daw_string_view.h"

#include <cassert>
#include <iostream>
#include <numeric>

void test_001( ) {
	std::array tbl = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	daw::InputRange rng{ tbl };
	auto result = std::accumulate( rng.begin( ), rng.end( ), 0 );
	assert( result == 45 );
	(void)result;
}

int main( int argc, char **argv ) {
	test_001( );
	int val = 0;
	for( int n = 0; n < argc; ++n ) {
		auto rng = daw::InputRange( daw::string_view( argv[n] ) );
		val += std::accumulate( rng.begin( ), rng.end( ), 0 );
	}
	std::cout << "Total: " << val << '\n';
}

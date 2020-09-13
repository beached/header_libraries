// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_keep_n.h"

#include <limits>

constexpr bool keep_n_test_001( ) {
	daw::keep_n<int, 3> top3( std::numeric_limits<int>::max( ) );
	top3.insert( 5 );
	top3.insert( 0 );
	top3.insert( 1 );
	top3.insert( 50 );
	top3.insert( -50 );

	daw::expecting( -50, top3[0] );
	daw::expecting( 0, top3[1] );
	daw::expecting( 1, top3[2] );
	return true;
}
static_assert( keep_n_test_001( ) );

int main( ) {}

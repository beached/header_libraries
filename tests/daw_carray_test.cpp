// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_carray.h"
#include "daw/daw_utility.h"

struct A {
	A( ) = delete;
};

void daw_heap_array_testing( ) {
	daw::carray<int, 6> t = { 1, 2, 3, 4, 5, 6 };
	daw::carray<A, 100> s{ };
	Unused( s );
	daw::expecting( 4, t[3] );
}

int main( ) {
	daw_heap_array_testing( );
}

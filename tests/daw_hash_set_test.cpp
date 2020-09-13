// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_hash_set.h"

#include <cstddef>

void test_001( ) {
	size_t count = 1024ULL;
	daw::hash_set_t<size_t> adapt( count );
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
}

void test_003( ) {
	size_t count = 1024ULL;
	daw::hash_set_t<size_t> adapt( count );
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		daw::expecting( adapt.exists( n ) );
	}
}

int main( ) {
	test_001( );
	test_003( );
}

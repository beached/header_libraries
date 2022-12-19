// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <daw/daw_unique_ptr.h>
#include <cassert>

int main( ) {
	auto a1 = daw::make_unique<int>( 5 );
	assert( a1 );
	assert( *a1 == 5 );
	auto a2 = daw::make_unique<int>( 5 );
	assert( a2 );
	assert( *a2 == 5 );
	assert( a1 != a2 );
	auto b = daw::make_unique<int[]>( 5 );
	assert( b );
	b[0] = 0;
	b[1] = 1;
	b[2] = 2;
	b[3] = 3;
	b[4] = 4;
	assert( b[1] == 1 );

}



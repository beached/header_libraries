// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_take.h>

#include <cassert>
#include <utility>

int main( ) {
	int a = 5;
	int b = daw::take( a );
	(void)a;
	(void)b;
	assert( a == 0 and b == 5 );

	auto x = daw::take_t<int>( 5 );
	auto y = x;
	assert( *x == 5 );
	auto z = std::move( y );
	assert( *y == 0 );
	assert( *z == 5 );
}

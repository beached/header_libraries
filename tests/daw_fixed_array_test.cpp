// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/deprecated/daw_fixed_array.h>

int main( ) {
	auto ary = daw::fixed_array<int>( 5 );
	ary[0] = 0;
	ary[1] = 1;
	ary[2] = 2;
	ary[3] = 3;
	ary[4] = 4;
	auto f = ary.begin( );
	auto fc = ary.cbegin( );
	++f;
	fc = f;
	assert( *f == 1 );
	assert( fc != ary.begin( ) );
}

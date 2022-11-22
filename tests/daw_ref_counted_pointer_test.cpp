// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ref_counted_pointer.h>

#include <cassert>

int main( ) {
	auto p0 = daw::rc_ptr<int>( new int{ 5 } );
	(void)p0;
	assert( *p0 == 5 );
	auto p1 = daw::rc_ptr<int[]>( new int[5]{ 1, 2, 3, 4, 5 } );
	(void)p1;
	assert( p1[4] == 5 );
}

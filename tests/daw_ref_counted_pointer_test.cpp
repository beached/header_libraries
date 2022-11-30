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
	auto p1b = p1;
	auto p1c = std::move( p1 );
	(void)p1;
	assert( p1[4] == 5 );
	auto p2b = p1;
	(void)p2b;
	assert( p2b[3] == 4 );
	auto p2c = std::move( p1 );
	assert( p2c != p1 );
	assert( p2c == p2b );
	(void)p2c;

	auto a1 = daw::make_rc_ptr<int[]>( 5 );
}

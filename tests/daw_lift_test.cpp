// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_lift.h>

#include <algorithm>
#include <array>
#include <cassert>

bool is_even( int a ) {
	return a % 2 == 0;
}

int main( ) {
	auto ary = std::array{ 1, 2, 3, 4 };
	std::partition( std::begin( ary ), std::end( ary ), DAW_LIFT( is_even ) );
	assert( ( is_even( ary[0] ) and is_even( ary[1] ) ) );
}

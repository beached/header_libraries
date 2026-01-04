// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bitset_helper.h"

#include <daw/daw_ensure.h>

#include <bitset>

int main( ) {
	auto const bs = daw::create_bitset_from_set_positions<16>( { 1, 2, 5, 10 } );
	daw_ensure( bs.count( ) == 4 );
	daw_ensure( bs.test( 1 ) );
	daw_ensure( bs.test( 2 ) );
	daw_ensure( bs.test( 5 ) );
	daw_ensure( bs.test( 10 ) );
}

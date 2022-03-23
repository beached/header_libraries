// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/vector.h>

#include <cstddef>
#include <iostream>

int main( ) {
	daw::vector<int> x;
	x.reserve( 3 );
	x = { 1, 2, 3 };
	x[0] = 2;
	std::cout << "cap: " << x.capacity( ) << '\n';
	x.resize_and_overwrite( x.capacity( ) * 2, []( int *p, std::size_t sz ) {
		for( std::size_t n = 0; n < sz; ++n ) {
			p[n] = n;
		}
		return sz;
	} );
	for( auto const &i : x ) {
		std::cout << i << '\n';
	}
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/iterator/daw_iota_iterator.h>

#include <iostream>

int main( ) {
	for( auto v : daw::iota_range( 0.12334, 19.2, 3.143540000000001 ) ) {
		std::cout << v << '\n';
	}
}

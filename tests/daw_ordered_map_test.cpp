// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_ordered_map.h"

#include "daw/daw_benchmark.h"

#include <string>

int main( ) {
	daw::ordered_map<std::string, int> dict{ };

	dict.insert( { "hello", 5 } );

	daw::expecting( !dict.empty( ) );
	daw::expecting( dict.size( ), 1U );
	daw::expecting( dict.front( ), dict.back( ) );
}

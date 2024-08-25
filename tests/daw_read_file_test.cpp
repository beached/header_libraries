// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_read_file.h"

#include "daw/daw_benchmark.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>

void daw_read_file_001( std::string s ) {
	auto f = daw::read_file( s );
	daw::expecting_message( f, "./daw_utility_test_bin does not exist" );
	std::cout << f->size( ) << '\n';
}

int main( int, char **argv ) {
	daw_read_file_001( argv[0] );
}

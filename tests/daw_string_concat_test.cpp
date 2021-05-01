// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_string_concat.h>

#include <iostream>

std::string concat2( std::string const &s0, std::string const &s1,
                     std::string const &s2 ) {
	return daw::string_concat( s0, s1, s2 );
}

int main( ) {
	std::cout << concat2( "A", "b", "c\n" );
}

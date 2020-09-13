// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/static_hash_table.h"

#include <iostream>

int main( int, char ** ) {
	constexpr static daw::static_hash_t<size_t, 2> const values = {
	  { "3", 0xdeadbeef }, { "5", 0xf00dbabe } };
	std::cout << values["3"] << std::endl;
	std::cout << values["5"] << std::endl;

	return 0;
}

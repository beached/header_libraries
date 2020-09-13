// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_hash_table2.h"

#include <cstdint>
#include <iostream>
#include <new>
#include <string>

using namespace std::literals::string_literals;
void daw_hash_table_testing( ) {
	daw::hash_table<int> testing1;
	struct big {
		uint32_t a;
		uint32_t b;
		uint16_t c;
	};
	daw::hash_table<big> testing2;

	testing1["hello"] = 5;
	testing1[454] = 6;
	testing1.shrink_to_fit( );
	testing2["hello"] = { 5, 6, 7 };
	testing2.shrink_to_fit( );
	auto a = sizeof( big );
	auto b = sizeof( big * );
	std::cout << a << b << testing1["hello"] << " " << testing1[454] << " "
	          << testing2["hello"].b << std::endl;
}

int main( ) {
	daw_hash_table_testing( );
}

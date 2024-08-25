// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/deprecated/daw_heap_array.h"

#include "daw/daw_benchmark.h"

#include <new>
#include <string>

using namespace std::literals::string_literals;

void daw_heap_array_testing( ) {
	daw::heap_array<int> t{ };
	t = { 1, 2, 3, 4, 5, 6 };

	auto pos = t.find_first_of( 4 );
	daw::expecting( 4, *pos );
}

int main( ) {
	daw_heap_array_testing( );
}

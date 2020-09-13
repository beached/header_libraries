// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <new>

#include "daw/daw_unique_array.h"

void daw_unique_array_test_001( ) {
	daw::unique_array_t<int> blah{ 15 };
}

int main( ) {
	daw_unique_array_test_001( );
}

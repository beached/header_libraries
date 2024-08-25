// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bounded_array.h"

constexpr bool daw_bounded_array_test_001( ) {
	daw::bounded_array_t<int, 5> a = { 1, 2, 3, 4, 5 };
	int sum = 0;
	for( auto c : a ) {
		sum += c;
	}
	return sum == 15;
}
static_assert( daw_bounded_array_test_001( ) );

int main( ) {}

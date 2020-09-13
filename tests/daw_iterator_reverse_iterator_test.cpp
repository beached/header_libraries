// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_algorithm.h"
#include "daw/iterator/daw_reverse_iterator.h"

#include <array>

constexpr bool reverse_iterator_test_001( ) {
	std::array<int, 6> a = { 1, 2, 3, 4, 5, 6 };
	std::array<int, 6> b{ };
	auto first = daw::make_reverse_iterator( a.end( ) );
	auto last = daw::make_reverse_iterator( a.begin( ) );
	auto first_out = daw::make_reverse_iterator( b.end( ) );
	daw::algorithm::copy( first, last, first_out );
	return daw::algorithm::equal( a.begin( ), a.end( ), b.begin( ) );
}
static_assert( reverse_iterator_test_001( ) );

int main( ) {}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_view.h"

#include <array>
#include <functional>

constexpr bool test_const_iterator_01( ) {
	using arry_t = std::array<int, 5>;
	arry_t arry = { 1, 2, 3, 4, 5 };
	daw::view a( arry.begin( ), arry.end( ) );
	daw::expecting( a.size( ) == 5 );
	daw::expecting( *arry.begin( ) == 1 );
	daw::expecting( a.size( ), arry.size( ) );

	daw::expecting(
	  daw::algorithm::equal( arry.begin( ), arry.end( ), a.begin( ), a.end( ) ) );

	return true;
}

static_assert( test_const_iterator_01( ) );

int main( ) {
	test_const_iterator_01( );
	return 0;
}

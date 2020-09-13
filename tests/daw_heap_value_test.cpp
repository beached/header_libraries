// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_heap_value.h"

#include <cstdint>
#include <iostream>

void daw_heap_value_test_01( ) {
	struct lrg {
		size_t a;
		size_t b;
		size_t c;
		size_t d;
		size_t e;
	};
	std::cout << "sizeof( int ) -> " << sizeof( int ) << '\t';
	std::cout << "sizeof( daw::heap_value<int> ) -> "
	          << sizeof( daw::heap_value<int> ) << '\n';
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t ) << '\t';
	std::cout << "sizeof( daw::heap_value<size_t> ) -> "
	          << sizeof( daw::heap_value<size_t> ) << '\n';
	std::cout << "sizeof( lrg ) -> " << sizeof( lrg ) << '\t';
	std::cout << "sizeof( daw::heap_value<lrg> ) -> "
	          << sizeof( daw::heap_value<lrg> ) << '\n';
	daw::heap_value<int> a{ };
	daw::heap_value<int> b{ 1 };
	daw::heap_value<int> c{ 2 };
	daw::heap_value<int> d{ 1 };

	// a & b
	daw::expecting( !( a == b ) );
	daw::expecting( !( b == a ) );
	daw::expecting( a != b );
	daw::expecting( b != a );
	daw::expecting( a < b );
	daw::expecting( !( b < a ) );
	daw::expecting( a <= b );
	daw::expecting( !( b <= a ) );
	daw::expecting( !( a >= b ) );
	daw::expecting( b >= a );
	daw::expecting( b == d );
	daw::expecting( b != c );
	daw::expecting( b < c );
	daw::expecting( b <= c );
	daw::expecting( c > b );
	daw::expecting( c >= b );
}

int main( ) {
	daw_heap_value_test_01( );
}

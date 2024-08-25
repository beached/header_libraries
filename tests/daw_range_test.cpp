// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_range.h"

#include <cstdint>
#include <iostream>
#include <vector>

// Workaround
#ifndef WIN32 // Fails on MSVC 2015.2
void daw_range_test01( ) {
	using namespace daw::range::operators;
	using daw::range::from;

	std::vector<int32_t> const t = {
	  -400, 4, -1, 1000, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	auto result = from( t ) << shuffle( ) << sort( ) << where( []( auto v ) {
		              return v > 0;
	              } ) << stable_partition( []( auto v ) {
		              return v % 2 == 0;
	              } );
	std::cout << "Test 001\n";
	std::cout << result << std::endl;

	std::cout << "{";
	for( auto const &v : result ) {
		std::cout << " " << v;
	}
	std::cout << " }\n";
}
#endif // WIN32

void daw_range_test02( ) {
	using daw::range::from;

	std::vector<int32_t> const t = {
	  -400, 4, -1, 1000, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	auto result = from( t )
	                .shuffle( )
	                .sort( )
	                .where( []( auto v ) {
		                return v > 0;
	                } )
	                .stable_partition( []( auto v ) {
		                return v % 2 == 0;
	                } );

	std::cout << "Test 002\n";
	std::cout << result << std::endl;

	std::cout << "{";
	for( auto v : result ) {
		std::cout << " " << v;
	}
	std::cout << " }\n";
}

int main( ) {
// Workaround
#ifndef WIN32
	daw_range_test01( );
#endif
	daw_range_test02( );
}

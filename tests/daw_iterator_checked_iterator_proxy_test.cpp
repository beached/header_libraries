// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_checked_iterator_proxy.h"

#include "daw/daw_benchmark.h"

#include <iostream>
#include <stdexcept>
#include <vector>

void daw_checked_iterator_proxy_001( ) {
	std::vector<int> const test = { 0, 1, 2, 3, 4 };

	for( auto it = daw::make_checked_iterator_proxy( test.begin( ), test.end( ) );
	     it != test.end( );
	     ++it ) {
		std::cout << *it << '\n';
	}
}

void daw_checked_iterator_proxy_002( ) {
	std::vector<int> const test = { 0, 1, 2, 3, 4 };

	bool result = false;
	try {
		for( auto it =
		       daw::make_checked_iterator_proxy( test.begin( ), test.begin( ) + 2 );
		     it != test.end( );
		     ++it ) {
			std::cout << *it << '\n';
		}
	} catch( std::out_of_range const &ex ) {
		result = true;
		std::cout << "Expected exception with message: " << ex.what( ) << '\n';
	}
	daw::expecting_message(
	  result, "Expected an out_of_range exception but didn't get one" );
}

int main( ) {
	daw_checked_iterator_proxy_001( );
	daw_checked_iterator_proxy_002( );
}

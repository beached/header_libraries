// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>

#include "daw/daw_benchmark.h"
#include "daw/iterator/daw_split_iterator.h"

void split_it_001( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );

	while( it != str.end( ) ) {
		std::cout << *it << '\n';
		++it;
	}
}

void split_it_002( ) {
	std::string str = "t  j ";
	auto it = daw::make_split_it( str, ' ' );

	while( it != str.end( ) ) {
		std::cout << *it << '\n';
		++it;
	}
}

void split_it_003( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );
	auto const last = it.make_end( );

	std::vector<std::string> results{ };
	std::transform( it, last, std::back_inserter( results ),
	                []( auto sv ) { return sv.to_string( ); } );

	for( auto const &s : results ) {
		std::cout << s << '\n';
	}
}

void split_it_004( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );
	auto const last = daw::split_it<char>{ };

	while( it != last ) {
		auto sv = *it;
		++it;
		--it;
		auto sv_prev = *it;
		daw::expecting( sv, sv_prev );
		++it;
	}
}

int main( ) {
	split_it_001( );
	split_it_002( );
	split_it_003( );
	split_it_004( );
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_zipcontainer.h"

#include <tuple>
#include <vector>

int main( ) {
	auto a = std::vector{ 1, 2, 3 };
	auto b = std::vector{ 9, 8, 7 };
	auto z = daw::zip_container( a, b );
	for( auto item : z ) {
		if( std::apply(
		      []( auto const &l, auto const &r ) {
			      return l + r;
		      },
		      item ) != 10 ) {
			std::abort( );
		}
	}
}

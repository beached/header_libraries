// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_visit_as.h>

int main( ) {
	float x = 3.14f;
	int y = daw::visit_as<int>( x, []( int & z ) { return z++; } );

	daw_ensure( x != 3.14f );
	daw_ensure( daw::bit_cast<float>( y ) != x );
}


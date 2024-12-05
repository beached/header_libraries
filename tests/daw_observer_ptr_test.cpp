// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_observer_ptr.h>
#include <daw/daw_ensure.h>

int main( int, char**argv ) {
	auto p = daw::observer_ptr( argv[0] );	
	daw_ensure( p == argv[0] );	
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_allow_once.h"

int foo( ) {
	static auto has_run = daw::allow_once_t{ };

	if( has_run ) {
		return 42;
	}
	return 0;
}

int main( ) {
	return foo( );
}
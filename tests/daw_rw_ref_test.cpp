// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_rw_ref.h"

int main( ) {
	int x = 42;
	auto r = daw::rw_ref{ x };
	r.get( ) = 0;
	return r;
}


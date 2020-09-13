// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_cstring.h"

void daw_cstring_01( ) {
	daw::cstring a = "a";
	daw::cstring aa = "aa";
	daw::cstring bb = "bb";

	daw::expecting( a == a );
	daw::expecting( a != aa );
	daw::expecting( a < aa );
	daw::expecting( bb > a );
}

int main( ) {
	daw_cstring_01( );
}

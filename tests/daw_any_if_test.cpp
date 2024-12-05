// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_any_if.h>

static_assert( daw::any_if([]{ return true;}, []{ return true; }));
static_assert( not daw::any_if([]{ return true;}, []{ return false; }));

int main( ) {
	
}

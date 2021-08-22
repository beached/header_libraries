// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//
//

#include <daw/daw_cx_offset_of.h>

struct A {
	char b;
	char c;
};

static_assert( daw::offset_of( &A::c ) == 1 );

int main( ) {
}

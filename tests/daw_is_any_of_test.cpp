// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_is_any_of.h>

static_assert( daw::is_any_of_v<1, 4, 3, 2, 1> );
static_assert( not daw::is_any_of_v<1, 4, 3, 2> );

int main( ) {}

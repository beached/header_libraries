// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_fwd_pack_apply.h>

struct Add {
	constexpr int operator( )( int a, int b ) const noexcept {
		return a + b;
	}
};

static_assert( daw::apply( Add{ }, daw::fwd_pack{ 1, 2 } ) == 3 );

int main( ) {}

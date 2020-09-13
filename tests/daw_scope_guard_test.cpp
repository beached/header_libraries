// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_scope_guard.h"

struct oe_t {
	bool *b;
	constexpr oe_t( bool *ptr ) noexcept
	  : b{ ptr } {}

	constexpr void operator( )( ) noexcept {
		*b = !*b;
	}
};

void scope_guard_001( ) {
	bool is_run = false;
	{ auto sg = daw::on_scope_exit( oe_t( &is_run ) ); }
	daw::expecting( is_run, true );
}

int main( ) {
	scope_guard_001( );
}

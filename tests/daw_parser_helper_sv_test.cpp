// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_parser_helper_sv.h"

constexpr bool test_in_t_test_001( ) {
	daw::parser::char_in_t<' ', '1'> tst{ };
	daw::expecting( tst( ' ' ) );
	daw::expecting( !tst( '#' ) );
	daw::expecting( tst( '1' ) );
	return true;
}
static_assert( test_in_t_test_001( ) );

int main( ) {}

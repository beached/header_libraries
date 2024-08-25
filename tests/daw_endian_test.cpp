// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_endian.h"

#include "daw/daw_benchmark.h"

#include <cstdint>

constexpr bool to_host_order_001( ) {
	if( daw::endian::native == daw::endian::little ) {
		constexpr uint16_t value = 0x1234;
		constexpr auto value2 = daw::to_big_endian( value );
		daw::expecting( value != value2 );
		daw::expecting( value2, 0x3412 );
	}
	return true;
}
static_assert( to_host_order_001( ) );

int main( ) {}

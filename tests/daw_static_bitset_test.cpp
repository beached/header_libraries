// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <bitset>
#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/daw_static_bitset.h"

static_assert( []( ) {
	daw::static_bitset<128> b(
	  "10000000000000000000000000000000000000000000000000000000000000000000000000"
	  "000000000000000000000000000000000000000000000000000000" );
	daw::expecting( 1U, b.one_count( ) );
	daw::expecting( 127U, b.zero_count( ) );
	return true;
}( ) );

bool test_001( ) noexcept {
	daw::static_bitset<128> b2(
	  0U, 0b1000000000000000000000000000000000000000000000000000000000000000 );
	return 127U == b2.zero_count( );
};
// static_assert( test_001( ) );

bool test_002( ) noexcept {
	daw::static_bitset<64> f( 0xFFFF'FFFF'FFFF'FFFF );
	f >>= 33U;
	daw::expecting( 31U, f.one_count( ) );
	return true;
};
// static_assert( test_002( ) );

int main( ) {
	auto tst2 = test_002( );
	daw::static_bitset<128> a( std::numeric_limits<uintmax_t>::max( ) );

	auto ac = a.one_count( );

	daw::static_bitset<64> e( 0xFFFF'FFFF'FFFF'FFFF );
	e <<= 5U;
	std::cout << e << '\n';

	daw::static_bitset<64> g( 0xFFFF'FFFF'FFFF'FFFF );
	g >>= 64;
	daw::expecting( 0U, g.one_count( ) );
	/*
	  static_assert( []( ) {
	    // Top bit masking to BitWidth
	    daw::static_bitset<16> h1( 0xDEAD'BEEF );
	    daw::static_bitset<16> h2( 0xBEEF );
	    daw::expecting( h1, h2 );
	    return true;
	  }( ) );*/

	std::cout << '\n';
}

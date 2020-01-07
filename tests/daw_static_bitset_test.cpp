// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw/boost_test.h"
#include "daw/daw_union_pair.h"

BOOST_AUTO_TEST_CASE( daw_union_pair_test_001 ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	struct vis_t {
		constexpr int operator( )( int i ) const noexcept {
			return i;
		}
		constexpr int operator( )( double d ) const noexcept {
			return static_cast<int>( d );
		}
	};
	BOOST_REQUIRE_EQUAL( 5, tmp.visit( vis_t( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_union_pair_test_002 ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	BOOST_REQUIRE_EQUAL(
	  5, tmp.visit( []( auto v ) { return static_cast<int>( v ); } ) );
}

BOOST_AUTO_TEST_CASE( daw_union_pair_test_003,
                      *boost::unit_test::tolerance( 0.1 ) ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	tmp = 6.6;
	struct vis_t {
		constexpr double operator( )( int i ) const noexcept {
			return static_cast<double>( i );
		}
		constexpr double operator( )( double d ) const noexcept {
			return d;
		}
	};
	BOOST_REQUIRE_EQUAL( 6.6, tmp.visit( vis_t( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_union_pair_test_004,
                      *boost::unit_test::tolerance( 0.1 ) ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	tmp = 6.6;
	BOOST_REQUIRE_EQUAL(
	  6.6, tmp.visit( []( auto v ) { return static_cast<double>( v ); } ) );
}

BOOST_AUTO_TEST_CASE( daw_union_pair_test_005 ) {
	auto tmp = daw::union_pair_t<int, double>( );
	auto tmp2 = daw::union_pair_t<int, double>( 5 );
	tmp = tmp2;
}

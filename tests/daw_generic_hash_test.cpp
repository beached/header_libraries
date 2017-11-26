// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "boost_test.h"
#include <iostream>

#include "daw_generic_hash.h"

BOOST_AUTO_TEST_CASE( test_01 ) {
	constexpr auto const c1 = daw::generic_hash( "Hello" );
	constexpr auto const c2 = daw::generic_hash( "Hello" );
	constexpr auto const c3 = daw::generic_hash( 5 );
	constexpr auto const c4 = daw::generic_hash( 0xFF00FF00FF00FF );
	std::cout << c1 << " " << c2 << " " << c3 << " " << c4 << '\n';
	BOOST_REQUIRE_EQUAL( c1, c2 );
	BOOST_REQUIRE( c1 != c3 );
}

/*
BOOST_AUTO_TEST_CASE( test_16_01 ) {
  constexpr auto const c1 = daw::generic_hash<2>( "Hello" );
  constexpr auto const c2 = daw::generic_hash<2>( "Hello" );
  constexpr auto const c3 = daw::generic_hash<2>( 5 );
  constexpr auto const c4 = daw::generic_hash<2>( 0xFF00FF00FF00FF );
  std::cout << c1 << " " << c2 << " " << c3 << " " << c4 << '\n';
  BOOST_REQUIRE_EQUAL( c1, c2 );
  BOOST_REQUIRE( c1 != c3 );
}
*/

BOOST_AUTO_TEST_CASE( test_32_01 ) {
	constexpr auto const c1 = daw::generic_hash<4>( "Hello" );
	constexpr auto const c2 = daw::generic_hash<4>( "Hello" );
	constexpr auto const c3 = daw::generic_hash<4>( 5 );
	constexpr auto const c4 = daw::generic_hash<4>( 0xFF00FF00FF00FF );
	std::cout << c1 << " " << c2 << " " << c3 << " " << c4 << '\n';
	BOOST_REQUIRE_EQUAL( c1, c2 );
	BOOST_REQUIRE( c1 != c3 );
}

BOOST_AUTO_TEST_CASE( test_64_01 ) {
	constexpr auto const c1 = daw::generic_hash<8>( "Hello" );
	constexpr auto const c2 = daw::generic_hash<8>( "Hello" );
	constexpr auto const c3 = daw::generic_hash<8>( 5 );
	constexpr auto const c4 = daw::generic_hash<8>( 0xFF00FF00FF00FF );
	std::cout << c1 << " " << c2 << " " << c3 << " " << c4 << '\n';
	BOOST_REQUIRE_EQUAL( c1, c2 );
	BOOST_REQUIRE( c1 != c3 );
}

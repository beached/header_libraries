// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost_test.h"
#include "daw_math.h"

void show( std::string value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << std::setiosflags( std::ios::fixed ) << std::setw( width + 2 )
	          << std::left << value << " ";
}

void show( double value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << " ";
	std::cout << std::setiosflags( std::ios::fixed ) << std::setprecision( width )
	          << std::right << value;
}

BOOST_AUTO_TEST_CASE( daw_math_factorial ) {
	std::cout << "factorial( 5 ) = " << daw::math::factorial( 5 ) << "\n";
	BOOST_REQUIRE_MESSAGE( daw::math::factorial( 5 ) == 120,
	                       "1. factorial of 5 should be 120" );
	std::cout << "factorial( 0 ) = " << daw::math::factorial( 0 ) << "\n";
	BOOST_REQUIRE_MESSAGE( daw::math::factorial( 0 ) == 1,
	                       "2. factorial of 0 should return 1" );
}

BOOST_AUTO_TEST_CASE( daw_math_round_001 ) {
	constexpr auto five = daw::math::round( 4.6 );

	BOOST_REQUIRE_EQUAL( five, 5 );
}

BOOST_AUTO_TEST_CASE( daw_math_round_by_001 ) {
	constexpr auto a = daw::math::round_by( 4.6, 7 );

	BOOST_REQUIRE_EQUAL( a, 7 );
}

BOOST_AUTO_TEST_CASE( daw_math_round_by_002 ) {
	constexpr auto a = daw::math::round_by( 4.6, 10 );

	BOOST_REQUIRE_EQUAL( a, 0 );
}

BOOST_AUTO_TEST_CASE( daw_math_ceil_by_001 ) {
	constexpr auto a = daw::math::ceil_by( 4.6, 1 );

	BOOST_REQUIRE_EQUAL( a, 5 );
}

BOOST_AUTO_TEST_CASE( daw_math_ceil_by_002 ) {
	constexpr auto a = daw::math::ceil_by( 4.6, 10 );

	BOOST_REQUIRE_EQUAL( a, 10 );
}

BOOST_AUTO_TEST_CASE( daw_math_ceil_by_003 ) {
	constexpr auto a = daw::math::ceil_by( 6.6, 10 );

	BOOST_REQUIRE_EQUAL( a, 10 );
}

BOOST_AUTO_TEST_CASE( daw_math_floor_by_001 ) {
	constexpr auto a = daw::math::floor_by( 4.6, 1 );

	BOOST_REQUIRE_EQUAL( a, 4 );
}

BOOST_AUTO_TEST_CASE( daw_math_floor_by_002 ) {
	constexpr auto a = daw::math::floor_by( 14.6, 10 );

	BOOST_REQUIRE_EQUAL( a, 10 );
}

BOOST_AUTO_TEST_CASE( daw_math_floor_by_003 ) {
	constexpr auto a = daw::math::floor_by( 6.6, 10 );

	BOOST_REQUIRE_EQUAL( a, 0 );
}

BOOST_AUTO_TEST_CASE( daw_math_nearly_equal_test ) {
	float f1 = 0;
	float f2 = 0;
	BOOST_REQUIRE( daw::math::nearly_equal( f1, f2 ) );
	f1 = 1.0;
	BOOST_REQUIRE( !daw::math::nearly_equal( f1, f2 ) );

	int i1 = 1;
	int i2 = 1;
	BOOST_REQUIRE( daw::math::nearly_equal( i1, i2 ) );
	i2 = 2;
	BOOST_REQUIRE( !daw::math::nearly_equal( i1, i2 ) );
}

BOOST_AUTO_TEST_CASE( daw_math_max_001 ) {
	BOOST_REQUIRE_EQUAL( daw::max( 5, 3, 100, 3, 4, 0xFFFF, -100 ), 0xFFFF );
}

BOOST_AUTO_TEST_CASE( daw_math_max_002 ) {
	BOOST_REQUIRE_EQUAL( daw::max( 5, 3, std::numeric_limits<float>::infinity( ),
	                               100, 3, 4, 0xFFFF, -100 ),
	                     std::numeric_limits<float>::infinity( ) );
}

BOOST_AUTO_TEST_CASE( daw_math_min_001 ) {
	BOOST_REQUIRE_EQUAL( daw::min( 5, 3, 100, 3, 4, -100, 0xFFFF ), -100 );
}

BOOST_AUTO_TEST_CASE( daw_math_min_002 ) {
	BOOST_REQUIRE_EQUAL( daw::min( 5, 3, 100,
	                               std::numeric_limits<float>::infinity( ), 3, 4,
	                               -100, 0xFFFF ),
	                     -100 );
}

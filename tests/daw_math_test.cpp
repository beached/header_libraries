// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "daw_math.h"

void show( std::string value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << std::setiosflags( std::ios::fixed ) << std::setw( width + 2 ) << std::left << value << " ";
}

void show( double value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << " ";
	std::cout << std::setiosflags( std::ios::fixed ) << std::setprecision( width ) << std::right << value;
}

BOOST_AUTO_TEST_CASE( daw_math_factorial ) {
	std::cout << "factorial( 5 ) = " << daw::math::factorial( 5 ) << "\n";
	BOOST_REQUIRE_MESSAGE( daw::math::factorial( 5 ) == 120, "1. factorial of 5 should be 120" );
	std::cout << "factorial( 0 ) = " << daw::math::factorial( 0 ) << "\n";
	BOOST_REQUIRE_MESSAGE( daw::math::factorial( 0 ) == 1, "2. factorial of 0 should return 1" );
}

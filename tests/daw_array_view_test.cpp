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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include "boost_test.h"
#include "daw_array_view.h"

BOOST_AUTO_TEST_CASE( daw_array_view_test_001 ) {
	constexpr auto const a = daw::make_array_view( "This is a test" );
	for( auto c : a ) {
		std::cout << c;
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_array_view_test_002 ) {
	int const arry_a[] = {1, 2, 3, 4, 5, 6, 7, 8};
	int const arry_b[] = {5, 6};
	auto const a = daw::make_array_view( arry_a );
	auto const b = daw::make_array_view( arry_b );
	auto const pos = a.find_first_of( b );

	BOOST_REQUIRE_EQUAL( pos, 4 );
}

BOOST_AUTO_TEST_CASE( daw_array_view_find_last_of_004 ) {
	int const arry_a[] = {1, 2, 3, 4, 5, 6, 7, 8};
	int const arry_b[] = {5, 6};
	auto const a = daw::make_array_view( arry_a );
	auto const b = daw::make_array_view( arry_b );
	auto const pos = a.find_last_of( b );

	BOOST_REQUIRE_EQUAL( pos, 4 );
}


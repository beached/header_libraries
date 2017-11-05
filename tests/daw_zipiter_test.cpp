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
#include <algorithm>
#include <iostream>
#include <cstdint>

#include "daw_static_array.h"
#include "daw_zipiter.h"

constexpr bool test( ) {
	daw::static_array_t<uint8_t, 10> a{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	daw::static_array_t<uint8_t, 10> b{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto zi = daw::make_zip_iterator( a.begin( ), b.begin( ) );
	++zi;
	return *get<0>( zi ) == 8 && *get<1>( zi ) == 2;
}


BOOST_AUTO_TEST_CASE( test_01 ) {
	constexpr bool result = test( );
	BOOST_REQUIRE( result );
}



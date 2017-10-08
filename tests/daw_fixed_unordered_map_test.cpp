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

#include "boost_test.h"
#include "daw_fixed_unordered_map.h"

BOOST_AUTO_TEST_CASE( daw_fixed_unordered_map_001 ) {
	daw::fixed_unordered_map<int, 100> blah{};
	blah['a'] = 5;
	blah["hello"] = 6;
	BOOST_REQUIRE_EQUAL( blah['a'], 5 );
	BOOST_REQUIRE_EQUAL( blah["hello"], 6 );
	BOOST_REQUIRE( blah['a'] != blah["hello"] );
	BOOST_REQUIRE_EQUAL( blah['a'], blah['a'] );
	std::cout << blah['a'] << " " << blah["hello"] << '\n';
}

constexpr auto get_map( ) {
	daw::fixed_unordered_map<int, 10> blah{};
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	return blah;
}

template<size_t N>
constexpr auto too_many( ) {
	daw::fixed_unordered_map<int, N> blah{};
	for( size_t n=0; n<=N; ++n ) {
		blah[n] = n;
	}
	return blah;
}

BOOST_AUTO_TEST_CASE( daw_fixed_unordered_map_002 ) {
	constexpr auto values2 = get_map( );
	daw::fixed_unordered_map<int, 10> blah{};
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	BOOST_REQUIRE_EQUAL( values2['a'], blah['a'] );
}

// Will not compile... on purpose
//BOOST_AUTO_TEST_CASE( daw_fixed_unordered_map_003 ) {
//	constexpr auto values2 = too_many<10>();
//}

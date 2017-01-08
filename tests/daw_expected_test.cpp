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
#include <iostream>

#include "daw_expected.h"

BOOST_AUTO_TEST_CASE( daw_expected_test_01 ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::expected_t<int> ) -> " << sizeof( daw::expected_t<int> );
	std::cout << " sizeof( daw::expected_t<size_t> ) -> " << sizeof( daw::expected_t<size_t> ) << '\n';
	daw::expected_t<int> a{ };
	daw::expected_t<int> b{ 1 };
	daw::expected_t<int> c{ 2 };
	daw::expected_t<int> d{ 1 };
	daw::expected_t<void> e;
	auto X = []( int x ) { std::cout << "Hello\n"; return x*x; };
	static_assert( daw::is_callable_v<decltype( X ), int>, "is_callable_v broke" );

	daw::expected_t<void> f{ []( ) { std::cout << "Hello\n"; } };
	daw::expected_t<void> g{ []( int ) { std::cout << "Hello\n"; }, 5 };
		//daw::expected_t<int> h{ []( int x ) { std::cout << "Hello\n"; return x*x; }, 5 };

	// a & b
	auto test_01 = !( a == b);
	auto test_02 = !( b == a);

	BOOST_REQUIRE( test_01 );
	BOOST_REQUIRE( test_02 );
}

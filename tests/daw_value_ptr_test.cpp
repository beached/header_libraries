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

#include <iostream>

#include "boost_test.h"
#include "daw_value_ptr.h"

BOOST_AUTO_TEST_CASE( daw_value_ptr_test_01 ) {
	struct lrg {
		size_t a;
		size_t b;
		size_t c;
		size_t d;
		size_t e;
	};
	std::cout << "sizeof( int ) -> " << sizeof( int ) << '\t';
	std::cout << "sizeof( daw::value_ptr<int> ) -> "
	          << sizeof( daw::value_ptr<int> ) << '\n';
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t ) << '\t';
	std::cout << "sizeof( daw::value_ptr<size_t> ) -> "
	          << sizeof( daw::value_ptr<size_t> ) << '\n';
	std::cout << "sizeof( lrg ) -> " << sizeof( lrg ) << '\t';
	std::cout << "sizeof( daw::value_ptr<lrg> ) -> "
	          << sizeof( daw::value_ptr<lrg> ) << '\n';
	auto a = daw::value_ptr<int>();
	auto b = daw::value_ptr<int>(1);
	auto c = daw::value_ptr<int>(2);
	auto d = daw::value_ptr<int>(1);

	// a & b
	auto test_01 = !( a == b );
	auto test_02 = !( b == a );
	auto test_03 = a != b;
	auto test_04 = b != a;
	auto test_05 = a < b;
	auto test_06 = !( b < a );
	auto test_07 = a <= b;
	auto test_08 = !( b <= a );
	auto test_09 = !( a >= b );
	auto test_10 = b >= a;
	auto test_11 = b == d;
	auto test_12 = b != c;
	auto test_13 = b < c;
	auto test_14 = b <= c;
	auto test_15 = c > b;
	auto test_16 = c >= b;

	BOOST_REQUIRE( test_01 );
	BOOST_REQUIRE( test_02 );
	BOOST_REQUIRE( test_03 );
	BOOST_REQUIRE( test_04 );
	BOOST_REQUIRE( test_05 );
	BOOST_REQUIRE( test_06 );
	BOOST_REQUIRE( test_07 );
	BOOST_REQUIRE( test_08 );
	BOOST_REQUIRE( test_09 );
	BOOST_REQUIRE( test_10 );
	BOOST_REQUIRE( test_11 );
	BOOST_REQUIRE( test_12 );
	BOOST_REQUIRE( test_13 );
	BOOST_REQUIRE( test_14 );
	BOOST_REQUIRE( test_15 );
	BOOST_REQUIRE( test_16 );

	struct A {
		constexpr A( ) noexcept {}
		A( A const & ) = delete;
		A & operator=( A const & ) = delete;
	};
	daw::value_ptr<A> e{ };

	auto f = std::move(e);

	auto g = daw::value_ptr<int>( );
	auto const h = g;

	g = 5;
	*h = 6;
}

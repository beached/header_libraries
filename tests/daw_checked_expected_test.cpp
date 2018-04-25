// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <iostream>
#include <stdexcept>

#include "boost_test.h"
#include "daw_checked_expected.h"

daw::checked_expected_t<int, std::runtime_error> divide( int v ) noexcept {
	try {
		if( v == 0 ) {
			throw std::runtime_error( "division by zero" );
		}
		return 4 / v;
	} catch( ... ) { return std::current_exception( ); }
}

daw::checked_expected_t<int, std::runtime_error> divide2( int v ) noexcept {
	try {
		if( v == 0 ) {
			throw std::runtime_error{ "division by zero" };
		}
		if( v > 4 ) {
			throw std::exception{};
		}
		return 4 / v;
	} catch( ... ) { return std::current_exception( ); }
}

BOOST_AUTO_TEST_CASE( daw_expected_test_01 ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::checked_expected_t<int> ) -> " << sizeof( daw::checked_expected_t<int> );
	std::cout << " sizeof( daw::checked_expected_t<size_t> ) -> " << sizeof( daw::checked_expected_t<size_t> ) << '\n';
	daw::checked_expected_t<int> a{};
	daw::checked_expected_t<int> b{1};
	daw::checked_expected_t<int> c{2};
	daw::checked_expected_t<int> d{1};
	daw::checked_expected_t<void> e;
	auto X = []( int x ) {
		std::cout << "Hello\n";
		return x * x;
	};
	static_assert( daw::is_callable_v<decltype( X ), int>, "is_callable_v broke" );

	daw::checked_expected_t<void> f{[]( ) { std::cout << "Hello\n"; }};
	daw::checked_expected_t<void> g{[]( int ) { std::cout << "Hello\n"; }, 5};
	// daw::checked_expected_t<int> h{ []( int x ) { std::cout << "Hello\n"; return x*x; }, 5 };

	auto h = divide( 0 );
	auto i = divide( 2 );
	auto h2 = divide2( 0 );
	auto i2 = divide2( 5 );
	BOOST_REQUIRE( h.has_exception( ) );
	BOOST_REQUIRE( !h.has_value( ) );
	BOOST_REQUIRE( h2.has_exception( ) );
	BOOST_REQUIRE( !h2.has_value( ) );
	BOOST_REQUIRE( !i.has_exception( ) );
	BOOST_REQUIRE( i.has_value( ) );
	// a & b
	auto test_01 = !( a == b );
	auto test_02 = !( b == a );

	daw::checked_expected_t<int> j{&divide, 0};
	BOOST_REQUIRE( j.has_exception( ) );
	BOOST_REQUIRE( !j.has_value( ) );

	BOOST_REQUIRE( test_01 );
	BOOST_REQUIRE( test_02 );

	struct L {
		int a;
	};

	daw::checked_expected_t<L> l{[]( ) { return L{5}; }};
	BOOST_REQUIRE( l->a == 5 );

}

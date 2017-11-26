// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#include "boost_test.h"
#include "daw_optional_poly.h"

BOOST_AUTO_TEST_CASE( daw_optional_poly_test_01 ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::optional_poly<int> ) -> " << sizeof( daw::optional_poly<int> );
	std::cout << " sizeof( daw::optional_poly<size_t> ) -> " << sizeof( daw::optional_poly<size_t> ) << '\n';
	daw::optional_poly<int> a{};
	daw::optional_poly<int> b{1};
	daw::optional_poly<int> c{2};
	daw::optional_poly<int> d{1};

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
}

BOOST_AUTO_TEST_CASE( daw_optional_poly_test_02 ) {
	struct test_t {
		int8_t a;
	};

	daw::optional_poly<test_t> tmp = test_t{};
	tmp->a = 5;
}

BOOST_AUTO_TEST_CASE( daw_optional_poly_test_03 ) {
	struct base_t {
		int blah;
		virtual ~base_t( ) = default;
		base_t( base_t const & ) = default;
		base_t( base_t && ) noexcept = default;
		base_t &operator=( base_t const & ) = default;
		base_t &operator=( base_t && ) noexcept = default;
		base_t( )
		  : blah{123} {}
		virtual int get( ) const {
			return blah;
		}
	};

	struct child_t : public base_t {
		int blah2;
		child_t( )
		  : blah2{321} {}
		explicit child_t( int x )
		  : blah2{x} {}
		~child_t( ) override = default;
		child_t( child_t const & ) = default;
		child_t( child_t && ) noexcept = default;
		child_t &operator=( child_t && ) noexcept = default;
		child_t &operator=( child_t const & ) = default;

		int get( ) const override {
			return blah2;
		}
	};

	daw::optional_poly<base_t> a = base_t{};
	daw::optional_poly<base_t> b = child_t{};

	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	b = child_t{4543};
	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	daw::optional_poly<child_t> c = child_t{54321};
	a = c;
	std::cout << a->get( ) << " " << b->get( ) << " " << c->get( ) << std::endl;
}

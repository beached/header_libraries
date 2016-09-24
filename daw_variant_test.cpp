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
#include <string>

#include "daw_variant.h"

BOOST_AUTO_TEST_CASE( daw_variant_001 ) {
	using namespace std::literals::string_literals;
	using v_t = daw::variant_t<int, float, std::string, int*>;
	std::cout << "size of variant is: " << sizeof( v_t ) << '\n';
	v_t s;
	v_t t = 5;
	std::string five = "5"s;
	v_t u = five; 
	BOOST_REQUIRE_MESSAGE( s.empty( ), "Default value should be empty" );
	BOOST_REQUIRE_MESSAGE( t.get<int>( ) == 5, "Value not there" );
	BOOST_REQUIRE_MESSAGE( static_cast<int>(t) == 5, "Value not there" );
	BOOST_REQUIRE_MESSAGE( t == 5, "Value not there" );
	BOOST_REQUIRE_MESSAGE( t.to_string( ) == "5", "too string not functioning" );
	BOOST_REQUIRE_MESSAGE( (t == u), "Type change" );
	BOOST_REQUIRE_MESSAGE( *t == "5", "operator* conversion" );
	BOOST_REQUIRE_MESSAGE( (t = 5.54f) == 5.54f, "Type change" );

	int * test_ptr = new int;
	*test_ptr = 1234;

	t = "hello"s;

	t = test_ptr;
	auto r = daw::get<int*>( t ) == test_ptr;
	BOOST_REQUIRE_MESSAGE( r, "Pointer" );

	{
		v_t test_i{ 5 };
		v_t test_s = std::string{ "hello" };
		BOOST_REQUIRE( test_i != test_s );
	}

}

struct blah {
	int a;
	int b;
};

std::string to_string( blah const & val ) {
	using std::to_string;
	return to_string( val.a ) + " " + to_string( val.b );
}

namespace std {
	std::string to_string( std::vector<int> const & v ) {
		std::string result;
		for( auto i: v ) {
			result += std::to_string( i );
		}
		return result;
	}
}

BOOST_AUTO_TEST_CASE( daw_variant_operators_001 ) {
	using var_t = daw::variant_t<int, blah>;

	var_t a = 5;
	var_t b = 6;
	var_t c = 6;
	var_t d = blah{ };

	BOOST_REQUIRE( a != b );
	BOOST_REQUIRE( a != d );
	BOOST_REQUIRE( d != a );
	BOOST_REQUIRE( b == c );
	BOOST_REQUIRE( a < b );
	BOOST_REQUIRE( a <= b );
	BOOST_REQUIRE( c <= b );
	BOOST_REQUIRE( b > a );
	BOOST_REQUIRE( b >= c );

	
	using var_t2 = daw::variant_t<std::vector<int>, int, double>;
	var_t2 e = std::vector<int>{ 1, 2, 3 };
	var_t2 f = std::vector<int>{ 1, 2, 3 };
	var_t2 g = std::vector<int>{ 1 };
	BOOST_REQUIRE( e == f );
	BOOST_REQUIRE( e != g );
}

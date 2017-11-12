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
#include "daw_fixed_stack.h"

BOOST_AUTO_TEST_CASE( daw_fixed_stack_test_001 ) {
	daw::fixed_stack_t<int, 10> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	int sum = 0;
	for( auto c : a ) {
		sum += c;
	}
	BOOST_REQUIRE_EQUAL( sum, 36 );
}

BOOST_AUTO_TEST_CASE( daw_fixed_stack_test_002 ) {
	daw::fixed_stack_t<int, 10> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	BOOST_REQUIRE_EQUAL( a.back( ), 4 );
	BOOST_REQUIRE_EQUAL( a.pop_back( ), 4 );
	BOOST_REQUIRE_EQUAL( a.back( ), 2 );
	BOOST_REQUIRE_EQUAL( a.size( ), 2 );
	BOOST_REQUIRE_EQUAL( a.capacity( ), 10 );
	BOOST_REQUIRE_EQUAL( a.front( ), 1 );
	BOOST_REQUIRE_EQUAL( *a.cbegin( ), 1 );
	BOOST_REQUIRE_EQUAL( a.pop_back( ), 2 );
	BOOST_REQUIRE_EQUAL( a.pop_back( ), 1 );
	BOOST_REQUIRE_EQUAL( a.size( ), 0 );
	BOOST_REQUIRE( a.empty( ) );
}

BOOST_AUTO_TEST_CASE( daw_fixed_stack_test_003 ) {
	daw::fixed_stack_t<int, 6> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	BOOST_REQUIRE( a.full( ) );
}

namespace daw_fixed_stack_test_004_ns {
	constexpr auto cx_test( ) {
		daw::fixed_stack_t<int, 10> a{};
		a.push_back( 1 );
		a.push_back( 2 );
		a.push_back( 4 );
		a.push_back( 8 );
		a.push_back( 16 );
		a.emplace_back( 5 );
		return a;
	}

	BOOST_AUTO_TEST_CASE( daw_fixed_stack_test_004 ) {
		constexpr auto a = cx_test( );
		int sum = 0;
		for( auto c : a ) {
			sum += c;
		}
		BOOST_REQUIRE_EQUAL( sum, 36 );
	}
} // namespace daw_fixed_stack_test_004_ns


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
#include <vector>

#include "boost_test.h"
#include "daw_circular_iterator.h"

BOOST_AUTO_TEST_CASE( daw_circular_iterator_001 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n=0; n<14; ++n ) {
		std::cout << *first++ << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_002 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n=0; n<14; ++n ) {
		std::cout << *first-- << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_003 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( intmax_t n=0; n<14; ++n ) {
		first += n*n;
		std::cout << *first << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_004 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_assignable = std::is_move_assignable<decltype( it )>::value;
	BOOST_REQUIRE( is_move_assignable );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_005 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_constructible = std::is_move_constructible<decltype( it )>::value;
	BOOST_REQUIRE( is_move_constructible );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_006 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_nothrow_move_assignable = std::is_nothrow_move_assignable<decltype( it )>::value;
	BOOST_CHECK( is_nothrow_move_assignable );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_007 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_nothrow_move_constructible = std::is_nothrow_move_constructible<decltype( it )>::value;
	BOOST_CHECK( is_nothrow_move_constructible );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_008 ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	using std::swap;
	swap( *it, *(it + 1) );
	BOOST_REQUIRE_EQUAL( a[0], 2 );
	BOOST_REQUIRE_EQUAL( a[1], 1 );
}


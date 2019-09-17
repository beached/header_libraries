// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_traits.h"
#include "daw/iterator/daw_circular_iterator.h"

BOOST_AUTO_TEST_CASE( daw_circular_iterator_001 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n = 0; n < 14; ++n ) {
		std::cout << *first++ << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_002 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n = 0; n < 14; ++n ) {
		std::cout << *first-- << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_003 ) {
	std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_circular_iterator( numbers );
	for( intmax_t n = 0; n < 14; ++n ) {
		first += n * n;
		std::cout << *first << ' ';
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_004 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_assignable =
	  std::is_move_assignable<decltype( it )>::value;
	daw::expecting( is_move_assignable );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_005 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_constructible =
	  std::is_move_constructible<decltype( it )>::value;
	daw::expecting( is_move_constructible );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_006 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_nothrow_move_assignable =
	  std::is_nothrow_move_assignable<decltype( it )>::value;
	BOOST_CHECK( is_nothrow_move_assignable );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_007 ) {
	std::vector<int> a;
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_nothrow_move_constructible =
	  std::is_nothrow_move_constructible<decltype( it )>::value;
	BOOST_CHECK( is_nothrow_move_constructible );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_008 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 4;
	constexpr bool tst1 = ::std::is_same_v<decltype( *it ), decltype( a[0] )>;
	constexpr bool tst2 = ::std::is_same_v<decltype( it ), decltype( it2 )>;
	daw::expecting( tst1 );
	daw::expecting( tst2 );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_009 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 4;
	constexpr bool tst1 = ::std::is_same_v<decltype( *it ), decltype( a[0] )>;
	constexpr bool tst2 = ::std::is_same_v<decltype( it ), decltype( it2 )>;
	daw::expecting( tst1 );
	daw::expecting( tst2 );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_010 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	using std::swap;
	swap( *it, *( it + 1 ) );
	daw::expecting( a[0], 2 );
	daw::expecting( a[1], 1 );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_011 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 3;
	daw::expecting( std::distance( it, it2 ), 3 );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_012 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 3;
	std::iter_swap( it, it2 );
	daw::expecting( a[0], 4 );
	daw::expecting( a[3], 1 );
}

BOOST_AUTO_TEST_CASE( daw_circular_iterator_013 ) {
	int a[] = {1, 2, 3, 4};
	auto it = daw::make_circular_iterator( a );
	auto const last = it.end( );
	std::reverse( it, last );
	daw::expecting( a[0], 4 );
	daw::expecting( a[1], 3 );
	daw::expecting( a[2], 2 );
	daw::expecting( a[3], 1 );
}

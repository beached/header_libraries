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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost_test.h"
#include "daw_algorithm.h"

BOOST_AUTO_TEST_CASE( daw_transform_many ) {
	std::vector<uint32_t> in1 = {1, 3, 5, 7, 9};
	std::vector<uint32_t> in2 = {0, 2, 4, 6, 8};
	std::vector<uint32_t> out;

	daw::algorithm::transform_many( in1.begin( ), in1.end( ), in2.begin( ), std::back_inserter( out ),
	                                []( auto const &v1, auto const &v2 ) { return v1 + v2; } );
	BOOST_REQUIRE_MESSAGE( out.size( ) == in1.size( ), "1. Incorrect size on output" );
}

constexpr bool quick_sort_test( ) noexcept {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::quick_sort( blah, blah + 6 );
	return daw::algorithm::is_sorted( blah, blah + 6 );
}

constexpr bool sort_test( ) noexcept {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::insertion_sort( blah, blah + 6 );
	return daw::algorithm::is_sorted( blah, blah + 6 );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_001 ) {
	constexpr bool sort_tst = sort_test( );
	BOOST_REQUIRE( sort_tst );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_002 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::insertion_sort( blah, blah + 6 );
	for( auto i : blah ) {
		std::cout << i << " ";
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_sort_test_003 ) {
	constexpr bool sort_tst = quick_sort_test( );
	BOOST_REQUIRE( sort_tst );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_004 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::quick_sort( blah, blah + 6 );
	for( auto i : blah ) {
		std::cout << i << " ";
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_map_test_001 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::map( blah, daw::algorithm::next( blah, 6 ), blah, []( auto val ) { return val % 2 == 0 ? 0 : 1; } );
}

constexpr int map_test_002( ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	struct {
		constexpr int operator( )( int val ) const noexcept {
			return val % 2 == 0 ? 0 : 1;
		}
	} unary_op{};

	daw::algorithm::map( blah, daw::algorithm::next( blah, 6 ), blah, unary_op );
	int result = 0;
	for( size_t n = 0; n < 6; ++n ) {
		result += blah[n];
	}
	return result;
}

BOOST_AUTO_TEST_CASE( daw_map_test_002 ) {
	constexpr auto const tst = map_test_002( );
	BOOST_REQUIRE_EQUAL( tst, 3 );
}

BOOST_AUTO_TEST_CASE( daw_reduce_test_001 ) {
	int blah[6] = {1, 0, 1, 0, 1, 0};
	auto const tst = daw::algorithm::reduce( blah, daw::algorithm::next( blah, 6 ),
	                                         0, []( auto lhs, auto rhs ) noexcept { return lhs + rhs; } );
	BOOST_REQUIRE_EQUAL( tst, 3 );
}

namespace copy_n_ns {
	constexpr bool test_copy_n( ) {
		char const src[] = "This is a test.";
		char dst[16] = {0};
		daw::algorithm::copy_n( src, dst, 16 );
		return true;
	}
	BOOST_AUTO_TEST_CASE( daw_copy_n_test_001 ) {
		constexpr auto b = test_copy_n( );
	}
} // namespace copy_n_ns

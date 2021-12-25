// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_range_algorithm.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>

namespace daw {
	void range_algorithm_accumulate( ) {
		std::vector<int> test( 100, 1 );
		auto sum = daw::algorithm::accumulate( test, 0 );
		daw::expecting_message( static_cast<int>( test.size( ) ) == sum,
		                        "A vector of 1's size should equal it's sum" );

		auto product = daw::algorithm::accumulate(
		  test, 1, []( int const &lhs, int const &rhs ) { return lhs * rhs; } );

		daw::expecting_message( 1 == product,
		                        "The product of a vector of 1's should be 1" );
	}

	void range_algorithm_map( ) {
		auto test_vec = std::vector<int>( 100 );
		std::iota( test_vec.begin( ), test_vec.end( ), 1 );

		auto result =
		  daw::algorithm::map( test_vec, []( int const &val ) { return 2 * val; } );

		daw::expecting_message(
		  test_vec.size( ) == result.size( ),
		  "Result of map should be of equal size to the input" );

		auto sum1 = daw::algorithm::accumulate( test_vec, 0 );
		auto sum2 = daw::algorithm::accumulate( result, 0 );

		daw::expecting_message(
		  sum2 == 2 * sum1,
		  "The result should have double the sum of the test_vec" );
	}

	void daw_range_algorithm_test_where1( ) {
		std::vector<int64_t> t1{ 1000 };
		std::iota( std::begin( t1 ), std::end( t1 ), 1 );
		auto result =
		  daw::algorithm::where( t1, []( auto v ) { return v % 2 == 0; } );
	}

	void daw_range_algorithm_test_sort( ) {
		std::vector<int64_t> v1{ 1000 };
		std::iota( std::begin( v1 ), std::end( v1 ), 1 );
		daw::algorithm::sort( v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
		daw::expecting( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
	}

	void daw_range_algorithm_test_stable_sort( ) {
		std::vector<int64_t> v1{ 1000 };
		std::iota( std::begin( v1 ), std::end( v1 ), 1 );
		daw::algorithm::stable_sort(
		  v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
		daw::expecting( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
	}

	void daw_range_algorithm_test_max_element( ) {
		std::vector<int64_t> v1{ 1000 };
		std::iota( std::begin( v1 ), std::end( v1 ), 1 );
		daw::algorithm::max_element(
		  v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
	}

	void daw_range_contains_001( ) {
		auto v = std::vector{1,2,3,4,5};
		auto p = daw::algorithm::contains( v, 4 );
		assert( p != std::end( v ) );
	}

	void daw_range_find_if_001( ) {
		auto v = std::vector{1,2,3,4,5};
		auto p = daw::algorithm::find_if( v, []( int i ) { return i % 2 == 0; } );
		assert( p != std::next( std::begin( v ), 1 ) );
	}
} // namespace daw

int main( ) {
	daw::range_algorithm_accumulate( );
	daw::range_algorithm_map( );
	daw::daw_range_algorithm_test_where1( );
	daw::daw_range_algorithm_test_sort( );
	daw::daw_range_algorithm_test_stable_sort( );
	daw::daw_range_algorithm_test_max_element( );
	daw::daw_range_contains_001( );
	daw::daw_range_find_if_001( );
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/iterator/daw_counting_iterators.h>

#include <daw/daw_benchmark.h>
#include <daw/daw_random.h>

#include <array>
#include <vector>

constexpr bool fwd_ary_test_001( ) {
	std::array nums = { 1, 2, 3, 4, 5, 6, 7 };
	auto first = daw::forward_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		++first;
	}
	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );

	return true;
}
static_assert( fwd_ary_test_001( ) );

constexpr bool fwd_ary_test_002( ) {
	std::array nums = { 1, 2, 3, 4, 5, 6, 7 };
	auto first = daw::forward_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		first++;
	}
	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );

	return true;
}
static_assert( fwd_ary_test_002( ) );

constexpr bool bidir_ary_test_001( ) {
	std::array nums = { 1, 2, 3, 4, 5, 6, 7 };
	auto first = daw::bidirectional_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		++first;
	}

	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );
	while( first != nums.begin( ) ) {
		--first;
	}
	daw::expecting( first.distance( ) == 0 );

	return true;
}
static_assert( bidir_ary_test_001( ) );

constexpr bool bidir_ary_test_002( ) {
	std::array nums = { 1, 2, 3, 4, 5, 6, 7 };
	auto first = daw::bidirectional_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		first++;
	}
	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );

	while( first != nums.begin( ) ) {
		first--;
	}
	daw::expecting( first.distance( ) == 0 );

	return true;
}
static_assert( bidir_ary_test_002( ) );

int main( ) {
	auto data = daw::make_random_data<int>( 1'000'000 );
	daw::bench_n_test<100>(
	  "Average Calc via counting iterator",
	  []( std::vector<int> values ) {
		  daw::do_not_optimize( values );
		  auto first = daw::forward_counting_iterator( values.begin( ) );
		  auto result = 0LL;
		  while( first != values.end( ) ) {
			  result += *first;
			  ++first;
		  }
		  daw::do_not_optimize( result );
		  return result / first.distance( );
	  },
	  data );
}

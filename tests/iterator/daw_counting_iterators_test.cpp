// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <daw/daw_algorithm.h>
#include <daw/daw_benchmark.h>
#include <daw/daw_random.h>
#include <daw/iterator/daw_counting_iterators.h>

constexpr bool fwd_ary_test_001( ) {
	std::array nums = {1, 2, 3, 4, 5, 6, 7};
	auto first = daw::forward_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		++first;
	}
	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );

	return true;
}
static_assert( fwd_ary_test_001( ) );

constexpr bool fwd_ary_test_002( ) {
	std::array nums = {1, 2, 3, 4, 5, 6, 7};
	auto first = daw::forward_counting_iterator( nums.begin( ) );

	while( first != nums.end( ) ) {
		first++;
	}
	daw::expecting( first.distance( ) == static_cast<int>( nums.size( ) ) );

	return true;
}
static_assert( fwd_ary_test_002( ) );

constexpr bool bidir_ary_test_001( ) {
	std::array nums = {1, 2, 3, 4, 5, 6, 7};
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
	std::array nums = {1, 2, 3, 4, 5, 6, 7};
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
		  	result+= *first;
		  	++first;
		  }
		  daw::do_not_optimize( result );
			return result / first.distance( );
	  },
	  data );
}

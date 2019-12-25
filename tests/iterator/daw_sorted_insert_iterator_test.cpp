// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_random.h"
#include "daw/iterator/daw_function_iterator.h"
#include "daw/iterator/daw_sorted_insert_iterator.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <vector>

using test_data_t = size_t;

void test_001( std::vector<test_data_t> const &data ) {
	std::vector<test_data_t> new_data{};
	auto oi = daw::make_sorted_insert_iterator( new_data );
	std::copy( begin( data ), end( data ), oi );
	daw::do_not_optimize( new_data );
}

void test_002( std::vector<test_data_t> const &data ) {
	std::vector<test_data_t> new_data{};
	std::copy( begin( data ), end( data ), std::back_inserter( new_data ) );
	std::sort( begin( new_data ), end( new_data ) );
	daw::do_not_optimize( new_data );
}

void test_003( std::vector<test_data_t> const &data ) {
	std::deque<test_data_t> new_data{};
	auto oi = daw::make_sorted_insert_iterator( new_data );
	std::copy( begin( data ), end( data ), oi );
	daw::do_not_optimize( new_data );
}

void test_004( std::vector<test_data_t> const &data ) {
	std::deque<test_data_t> new_data{};
	std::copy( begin( data ), end( data ), std::back_inserter( new_data ) );
	std::sort( begin( new_data ), end( new_data ) );
	daw::do_not_optimize( new_data );
}

void test_005( std::vector<test_data_t> const &data ) {
	std::list<test_data_t> new_data{};
	auto oi = daw::make_sorted_insert_iterator( new_data );
	std::copy( begin( data ), end( data ), oi );
	daw::do_not_optimize( new_data );
}

template<typename... Args>
void test_006( std::vector<test_data_t> const &data ) {
	std::list<test_data_t> new_data{};
	std::copy( begin( data ), end( data ),
	           std::inserter( new_data, end( new_data ) ) );
	new_data.sort( );
	daw::do_not_optimize( new_data );
}

int main( ) {
	for( size_t sz = 10ULL; sz < 1'000ULL; sz *= 10 ) {
		std::vector<test_data_t> test_data( sz, 0ULL );
		daw::random_fill<test_data_t>( begin( test_data ), end( test_data ), 0ULL,
		                               std::numeric_limits<test_data_t>::max( ) );
		std::cout << "Data size: " << sz << '\n';
		daw::bench_n_test<100>(
		  "vector<size_t> Insert sorted iterator",
		  []( std::vector<size_t> const &data ) { test_001( data ); }, test_data );
		daw::bench_n_test<100>(
		  "vector<size_t> Insert/Sort",
		  []( std::vector<size_t> const &data ) { test_002( data ); }, test_data );

		daw::bench_n_test<100>(
		  "deque<size_t> Insert sorted iterator",
		  []( std::vector<size_t> const &data ) { test_003( data ); }, test_data );
		daw::bench_n_test<100>(
		  "deque<size_t> Insert/Sort",
		  []( std::vector<size_t> const &data ) { test_004( data ); }, test_data );

		daw::bench_n_test<100>(
		  "list<size_t> Insert sorted iterator",
		  []( std::vector<size_t> const &data ) { test_005( data ); }, test_data );
		daw::bench_n_test<100>(
		  "list<size_t> Insert/Sort",
		  []( std::vector<size_t> const &data ) { test_006( data ); }, test_data );
		std::cout << "####################\n";
	}
}

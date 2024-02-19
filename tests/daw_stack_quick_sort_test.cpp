// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_random.h"
#include "daw/daw_stack_quick_sort.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

using value_type = uint64_t;
inline size_t constexpr MAX_VALUES = 1'048'576U;

auto const &get_random_data( ) {
	static auto const rnd_values =
	  daw::make_random_data<value_type>( MAX_VALUES );
	return rnd_values;
}

template<typename T>
std::vector<T> copy_n_values( std::vector<T> const &vals, size_t n ) {
	std::vector<T> result{ };
	result.reserve( n );
	std::copy_n( vals.cbegin( ), n, std::back_inserter( result ) );
	return result;
}

template<size_t N>
void test_perf_comp( ) {
	auto const values = copy_n_values( get_random_data( ), N );
	daw::do_not_optimize( values );
	auto const validator = []( auto const &v ) {
		auto unsorted_pos = std::adjacent_find( v.begin( ), v.end( ),
		                                        []( auto const &l, auto const &r ) {
			                                        return r < l;
		                                        } );
		if( unsorted_pos == v.end( ) ) {
			return true;
		}
		auto const dist = std::distance( v.begin( ), unsorted_pos );
		(void)dist;
		return false;
	};

	auto const bytes = values.size( ) * sizeof( value_type );
	daw::bench_n_test_mbs2<500>(
	  "stack sort", bytes, validator,
	  []( auto v ) {
		  daw::quick_sort( v.begin( ), v.end( ) );
		  daw::do_not_optimize( v );
		  return std::move( v );
	  },
	  values );

	daw::bench_n_test_mbs2<500>(
	  "std::sort", bytes, validator,
	  []( auto v ) {
		  std::sort( v.begin( ), v.end( ) );
		  daw::do_not_optimize( v );
		  return std::move( v );
	  },
	  values );
}

template<size_t N>
void do_test( ) {
	static_assert( N <= MAX_VALUES );
	std::cout << N << " items\n";
	test_perf_comp<N>( );
}

int main( int argc, char **argv ) {
	do_test<1>( );
	do_test<2>( );
	do_test<4>( );
	do_test<8>( );
	do_test<16>( );
	do_test<32>( );
	do_test<64>( );
	do_test<128>( );
	do_test<256>( );
	do_test<512>( );
	do_test<1024>( );
	do_test<16'384>( );
	do_test<131'072>( );
	do_test<MAX_VALUES>( );
}

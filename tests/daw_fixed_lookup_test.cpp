// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/deprecated/daw_fixed_lookup.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>

void daw_fixed_lookup_001( ) {
	daw::fixed_lookup<int, 100> blah{ };
	blah['a'] = 5;
	blah["hello"] = 6;
	daw::expecting( blah['a'], 5 );
	daw::expecting( blah["hello"], 6 );
	daw::expecting( blah['a'] != blah["hello"] );
	daw::expecting( blah['a'], blah['a'] );
	std::cout << blah['a'] << " " << blah["hello"] << '\n';
}

constexpr auto get_map( ) {
	daw::fixed_lookup<int, 10> blah{ };
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	return blah;
}

// Will not compile... on purpose
template<size_t N>
constexpr auto too_many( ) {
	daw::fixed_lookup<int, N> blah{ };
	for( size_t n = 0; n <= N; ++n ) {
		blah[n] = n;
	}
	return blah;
}

constexpr bool daw_fixed_lookup_002( ) {
	constexpr auto values2 = get_map( );
	daw::fixed_lookup<int, 10> blah{ };
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	daw::expecting( values2['a'], blah['a'] );
	return true;
}
static_assert( daw_fixed_lookup_002( ) );

constexpr bool daw_make_fixed_lookup_001( ) {
	auto values = daw::make_fixed_lookup<int>( "hello", 3, 5, 6, "why oh why" );
	daw::expecting( values.size( ), values.capacity( ) );
	daw::expecting( 5U, values.size( ) );
	return true;
}
// WORKAROUND
#ifndef WIN32
static_assert( daw_make_fixed_lookup_001( ) );
#endif

template<typename ValueType, ValueType SZ, size_t HashSize>
void do_test( ) {
	std::cout << "Testing with SZ = " << SZ << " and hash_size = " << HashSize
	          << '\n';
	using value_t = ValueType;
	daw::fixed_lookup<value_t, SZ * 2> lookup{ };
	std::unordered_map<value_t, value_t> hash_map{ };
	std::vector<value_t> ary{ };
	ary.resize( SZ );
	hash_map.reserve( SZ );
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "fixed_lookup(fill)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  lookup[n] = n;
		  }
	  },
	  2,
	  2,
	  SZ );
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "unordered_map(fill)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  hash_map[n] = n;
		  }
	  },
	  2,
	  2,
	  SZ );
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "vector(fill)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  ary[static_cast<size_t>( n )] = n;
		  }
	  },
	  2,
	  2,
	  SZ );

	intmax_t sum1 = 0;
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "fixed_lookup(summation)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  sum1 += lookup[n];
		  }
	  },
	  2,
	  2,
	  SZ );
	intmax_t sum2 = 0;
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "unordered_map(summation)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  sum2 += hash_map[n];
		  }
	  },
	  2,
	  2,
	  SZ );
	intmax_t sum3 = 0;
	daw::show_benchmark(
	  SZ * sizeof( value_t ),
	  "vector(summation)",
	  [&]( ) {
		  for( value_t n = 0; n < SZ; ++n ) {
			  sum3 += ary[static_cast<size_t>( n )];
		  }
	  },
	  2,
	  2,
	  SZ );
	daw::expecting( sum1, sum2 );
	daw::expecting( sum1, sum3 );
	std::cout << "sum1: " << sum1 << " sum2: " << sum2 << " sum3: " << sum3
	          << '\n';
}

void daw_fixed_lookup_bench_001( ) {
	do_test<int64_t, 15'000, 8>( );
	do_test<int32_t, 30'000, 4>( );
}

int main( ) {
	daw_make_fixed_lookup_001( );
	daw_fixed_lookup_001( );
	daw_fixed_lookup_bench_001( );
}

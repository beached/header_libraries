// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#define USE_CXSEED
#include "daw/daw_random.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

void daw_random_01( ) {
	for( auto n = 0; n < 100000; ++n ) {
		auto v1 = daw::randint( 0, 1000 );
		daw::expecting( 0 <= v1 and v1 <= 1000 );
	}
}

void daw_random_02( ) {
	for( auto n = 0; n < 100000; ++n ) {
		auto v1 = daw::randint<int64_t>( std::numeric_limits<int64_t>::min( ),
		                                 std::numeric_limits<int64_t>::max( ) );
		daw::expecting( std::numeric_limits<int64_t>::min( ) <= v1 and
		                v1 <= std::numeric_limits<int64_t>::max( ) );
	}
}

void daw_shuffle_01( ) {
	std::vector<int32_t> a;
	daw::shuffle( a.begin( ), a.end( ) );
	a.reserve( 40 );
	for( int32_t n = 0; n < 40; ++n ) {
		std::cout << " " << n;
		a.push_back( n );
	}
	std::cout << "\n\nshuffled:\n";
	daw::shuffle( a.begin( ), a.end( ) );
	for( int32_t const &i : a ) {
		std::cout << " " << i;
	}
	std::cout << '\n';
}

void daw_fill_01( ) {
	std::vector<int32_t> a;
	a.resize( 40 );
	std::cout << "Before: \n";
	for( int32_t const &i : a ) {
		std::cout << " " << i;
	}
	daw::random_fill( a.begin( ), a.end( ), 0, 100 );
	std::cout << "\n\nAfter: \n";
	for( int32_t const &i : a ) {
		std::cout << " " << i;
	}
	std::cout << '\n';
}

void daw_make_random_01( ) {
	using rnd_t = int16_t;
	auto const r = daw::make_random_data<rnd_t>( 40, 1, 6 );
	std::cout << "Generated Data: \n";
	for( auto const &i : r ) {
		std::cout << " " << i;
	}
	std::cout << "\n\n";
}

constexpr bool cxrand_test_001( ) {
	auto rng = daw::static_random( );
	return rng( ) != rng( );
}
static_assert( cxrand_test_001( ) );

void cxrand_test_002( ) {
	auto rng = daw::static_random( );
	for( size_t n = 0; n < 50; ++n ) {
		size_t num = rng( );
		num = ( ( num >> 60U ) | ( num & 0xFF ) ) % 100U;
		std::cout << " " << num;
	}
	std::cout << '\n';
}

int main( ) {
	daw_random_01( );
	daw_random_02( );
	daw_shuffle_01( );
	daw_fill_01( );
	daw_make_random_01( );
	cxrand_test_002( );
}

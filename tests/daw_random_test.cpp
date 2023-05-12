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
		auto v1 = daw::randint<int64_t>( -12345, 12345 );
		daw::expecting( -12345 <= v1 and v1 <= 12345 );
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

void random_class_integer_bench_uint64( ) {
	auto rnd = daw::RandomInteger<std::uint64_t>( );
	auto values = std::vector<std::uint64_t>( );
	values.resize( 100'000ULL );
	daw::bench_n_test<250>( "Random Number Class - uint64 * 100,000", [&] {
		for( std::size_t n = 0; n < 100'000ULL; ++n ) {
			values[n] = rnd( );
		}
		daw::do_not_optimize( values );
		daw::do_not_optimize( values.data( ) );
	} );

	auto rnd2 = daw::RandomInteger<std::uint64_t, std::mt19937_64>( );
	daw::bench_n_test<250>(
	  "Random Number Class - mersenne twister - uint64 * 100,000", [&] {
		  for( std::size_t n = 0; n < 100'000ULL; ++n ) {
			  values[n] = rnd2( );
		  }
		  daw::do_not_optimize( values );
		  daw::do_not_optimize( values.data( ) );
	  } );
}

void random_class_integer_bench_uint32( ) {
	auto rnd = daw::RandomInteger<std::uint32_t>( );
	auto values = std::vector<std::uint32_t>( );
	values.resize( 100'000ULL );
	daw::bench_n_test<250>( "Random Number Class - uint32 * 100,000", [&] {
		for( std::size_t n = 0; n < 100'000ULL; ++n ) {
			values[n] = rnd( );
		}
		daw::do_not_optimize( values );
		daw::do_not_optimize( values.data( ) );
	} );
}

void random_class_integer_bench_double( ) {
	auto rnd = daw::RandomFloat<double>( );
	auto values = std::vector<double>( );
	values.resize( 100'000ULL );
	daw::bench_n_test<250>( "Random Number Class - double * 100,000", [&] {
		for( std::size_t n = 0; n < 100'000ULL; ++n ) {
			values[n] = rnd( );
		}
		daw::do_not_optimize( values );
		daw::do_not_optimize( values.data( ) );
	} );
}

void random_class_integer_bench_float( ) {
	auto rnd = daw::RandomFloat<float>( );
	auto values = std::vector<float>( );
	values.resize( 100'000ULL );
	daw::bench_n_test<250>( "Random Number Class - float * 100,000", [&] {
		for( std::size_t n = 0; n < 100'000ULL; ++n ) {
			values[n] = rnd( );
		}
		daw::do_not_optimize( values );
		daw::do_not_optimize( values.data( ) );
	} );
}

template<typename Integer>
void show_dist( std::vector<Integer> const &v,
                typename daw::traits::identity<Integer>::type minimum,
                typename daw::traits::identity<Integer>::type maximum ) {
	auto bins = std::vector<std::size_t>{ };
	auto const sz = maximum - minimum + 1;
	bins.resize( sz );
	for( auto val : v ) {
		++bins[val - minimum];
	}
	for( std::size_t n = 0; n < bins.size( ); ++n ) {
		std::cout << ( static_cast<Integer>( n ) + minimum ) << ": " << bins[n]
		          << "-> "
		          << ( static_cast<float>( bins[n] * 1000 / v.size( ) ) / 10.0f )
		          << "%\n";
	}
}

void daw_make_random_02( ) {
	using uint_t = std::uint64_t;
	auto rnd = daw::RandomInteger<uint_t>( );
	auto r = std::vector<uint_t>( );
	constexpr auto minimum = 0;
	constexpr auto maximum = 128;
	constexpr auto sz = maximum - minimum + 1;
	(void)sz;
	r.resize( 1'000'000ULL );
	for( auto &v : r ) {
		v = rnd( maximum, minimum );
	}
	std::cout << "Generated Data: \n";
	show_dist( r, minimum, maximum );
	std::cout << "\n\n";
}

int main( ) {
	daw_random_01( );
	daw_random_02( );
	daw_shuffle_01( );
	daw_fill_01( );
	daw_make_random_01( );
	cxrand_test_002( );
	random_class_integer_bench_uint64( );
	random_class_integer_bench_uint32( );
	random_class_integer_bench_float( );
	random_class_integer_bench_double( );
	daw_make_random_02( );
}

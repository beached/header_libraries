// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_observable_ptr.h"

#include <atomic>
#include <cstdint>

void test_001( ) {
	int *p = new int{ 4 };
	auto t = daw::observable_ptr<int>( p );

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	daw::expecting( 5, *p );
}

void test_002( ) {
	int *p = new int{ 4 };
	auto t = daw::observable_ptr<int>( p );

	auto obs = t.get_observer( );
	{
		auto lck = obs.try_borrow( );
		if( lck ) {
			*lck = 5;
		}
	}
	daw::expecting( 5, *p );
}

void test_003( ) {
	auto t = daw::make_observable_ptr<int>( 4 );

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	daw::expecting( 5, *t );
}

void test_004( ) {
	auto t = daw::make_observable_ptr<std::atomic_int_least8_t>(
	  static_cast<int_least8_t>( 0 ) );

	auto const obs = t.get_observer( );

	daw::expecting( 0, obs.borrow( )->load( ) );
}

namespace {
	struct test_005_t {
		daw::observable_ptr<std::atomic_int_least8_t> value;

		test_005_t( )
		  : value{ daw::make_observable_ptr<std::atomic_int_least8_t>(
		      static_cast<int_least8_t>( 0 ) ) } {}
	};
} // namespace

void test_005( ) {
	test_005_t tmp{ };
	auto v = tmp.value->load( );
	daw::expecting( 0, v );
}

int main( ) {
	test_001( );
	test_002( );
	test_003( );
	test_004( );
	test_005( );
}

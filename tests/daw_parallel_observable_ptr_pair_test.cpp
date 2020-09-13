// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_observable_ptr.h"
#include "daw/parallel/daw_observable_ptr_pair.h"

#include <atomic>
#include <new>

void test_001( ) {
	int *p = new int{ 4 };
	daw::observable_ptr_pair<int> t{ p };

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	daw::expecting( 5, *p );
}

void test_002( ) {
	int *p = new int{ 4 };
	daw::observable_ptr_pair<int> t{ p };

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
	auto t = daw::make_observable_ptr_pair<int>( 4 );

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	daw::expecting( 5, *t );
}

void test_004( ) {

	auto t =
	  daw::make_observable_ptr_pair<std::atomic_int>( static_cast<int>( 0 ) );

	auto const obs = t.get_observer( );

	daw::expecting( 0, obs.borrow( )->load( ) );
}

namespace {
	struct test_t {
		int value;

		test_t( int v )
		  : value{ v } {}

		void set_value( int v ) {
			value = v;
		}
	};
} // namespace

void test_operator_arrow_005( ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto v = tmp->value;
	daw::expecting( 5, v );
}

void test_visit_006( ) {
	auto const tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto const res = tmp.visit( []( auto v ) { return v.value; } );
	daw::expecting( 5, res );
}

void test_apply_visitor_007( ) {
	auto const tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto const res =
	  tmp.apply_visitor( []( auto const &ptr ) { return ptr->value; } );
	daw::expecting( 5, res );
}

void test_visit_008( ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	tmp.visit( []( auto &v ) -> int & { return v.value; } ) = 6;
	auto const res = tmp.visit( []( auto v ) -> int { return v.value; } );
	daw::expecting( 6, res );
}

void test_visit_009( ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	tmp.visit( []( auto &v ) { v.set_value( 6 ); } );
	auto const res = tmp.visit( []( auto v ) -> int { return v.value; } );
	daw::expecting( 6, res );
}

int main( ) {
	test_001( );
	test_002( );
	test_003( );
	test_004( );
	test_operator_arrow_005( );
	test_visit_006( );
	test_apply_visitor_007( );
	test_visit_008( );
	test_visit_009( );
}

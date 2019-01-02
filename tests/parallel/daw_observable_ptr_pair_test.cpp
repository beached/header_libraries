// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include "daw/boost_test.h"
#include <iostream>

#include "daw/parallel/daw_observable_ptr_pair.h"

BOOST_AUTO_TEST_CASE( test_001 ) {
	int *p = new int{4};
	daw::observable_ptr_pair<int> t{p};

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	BOOST_REQUIRE_EQUAL( *p, 5 );
}

BOOST_AUTO_TEST_CASE( test_002 ) {
	int *p = new int{4};
	daw::observable_ptr_pair<int> t{p};

	auto obs = t.get_observer( );
	{
		auto lck = obs.try_borrow( );
		if( lck ) {
			*lck = 5;
		}
	}
	BOOST_REQUIRE_EQUAL( *p, 5 );
}

BOOST_AUTO_TEST_CASE( test_003 ) {
	auto t = daw::make_observable_ptr_pair<int>( 4 );

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	BOOST_REQUIRE_EQUAL( *t, 5 );
}

BOOST_AUTO_TEST_CASE( test_004 ) {

	auto t =
	  daw::make_observable_ptr_pair<std::atomic_int>( static_cast<int>( 0 ) );

	auto const obs = t.get_observer( );

	BOOST_REQUIRE_EQUAL( obs.borrow( )->load( ), 0 );
}

namespace {
	struct test_t {
		int value;

		test_t( int v )
		  : value{v} {}

		void set_value( int v ) {
			value = v;
		}
	};
} // namespace

BOOST_AUTO_TEST_CASE( test_operator_arrow_005 ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto v = tmp->value;
	BOOST_REQUIRE_EQUAL( 5, v );
}

BOOST_AUTO_TEST_CASE( test_visit_006 ) {
	auto const tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto const res = tmp.visit( []( auto v ) { return v.value; } );
	BOOST_REQUIRE_EQUAL( 5, res );
}

BOOST_AUTO_TEST_CASE( test_apply_visitor_007 ) {
	auto const tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	auto const res =
	  tmp.apply_visitor( []( auto const &ptr ) { return ptr->value; } );
	BOOST_REQUIRE_EQUAL( 5, res );
}

BOOST_AUTO_TEST_CASE( test_visit_008 ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	tmp.visit( []( auto &v ) -> int & { return v.value; } ) = 6;
	auto const res = tmp.visit( []( auto v ) -> int { return v.value; } );
	BOOST_REQUIRE_EQUAL( 6, res );
}

BOOST_AUTO_TEST_CASE( test_visit_009 ) {
	auto tmp = daw::make_observable_ptr_pair<test_t>( 5 );
	tmp.visit( []( auto &v ) { v.set_value( 6 ); } );
	auto const res = tmp.visit( []( auto v ) -> int { return v.value; } );
	BOOST_REQUIRE_EQUAL( 6, res );
}

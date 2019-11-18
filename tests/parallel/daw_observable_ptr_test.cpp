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

#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_observable_ptr.h"

void test_001( ) {
	int *p = new int{4};
	auto t = daw::observable_ptr<int>( p );

	auto obs = t.get_observer( );

	obs.lock( []( int &val ) { val = 5; } );

	daw::expecting( 5, *p );
}

void test_002( ) {
	int *p = new int{4};
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
		  : value{daw::make_observable_ptr<std::atomic_int_least8_t>(
		      static_cast<int_least8_t>( 0 ) )} {}
	};
} // namespace

void test_005( ) {
	test_005_t tmp{};
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

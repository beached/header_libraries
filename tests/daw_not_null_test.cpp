// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_not_null.h"

#include <cassert>
#include <memory>
#include <optional>

void daw_not_null_test_001( ) {
	auto tst = std::make_unique<int>( 5 );
	daw::not_null<int *> t = tst.release( );
	auto tst2 = std::unique_ptr<int>( t );
}

void daw_not_null_test_002( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	daw::not_null<int *> first = tst.get( );
	daw::not_null<int *> last = tst.get( ) + 5;
	assert( ( last - first ) == 5 );
}

void daw_not_null_test_003( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	daw::not_null<int *> first = tst.get( );
	int *last = tst.get( ) + 5;
	assert( ( last - first ) == 5 );
}

void daw_not_null_test_004( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	int *first = tst.get( );
	daw::not_null<int *> last = tst.get( ) + 5;
	assert( ( last - first ) == 5 );
}

int main( ) {
	daw_not_null_test_001( );
	daw_not_null_test_002( );
	daw_not_null_test_003( );
	daw_not_null_test_004( );
}

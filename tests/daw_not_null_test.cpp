// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_not_null.h"

#include "daw/daw_benchmark.h"
#include "daw/daw_ensure.h"

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
	(void)first;
	(void)last;
	daw_ensure( ( last - first ) == 5 );
}

void daw_not_null_test_003( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	daw::not_null<int *> first = tst.get( );
	int *last = tst.get( ) + 5;
	(void)first;
	(void)last;
	daw_ensure( ( last - first ) == 5 );
}

void daw_not_null_test_004( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	int *first = tst.get( );
	daw::not_null<int *> last = tst.get( ) + 5;
	(void)first;
	(void)last;
	daw_ensure( ( last - first ) == 5 );
}

#if defined( __cpp_constexpr_dynamic_alloc )
#if __cpp_constexpr_dynamic_alloc >= 201907L
constexpr bool daw_not_null_test_004c( ) {
	auto tst = std::unique_ptr<int[]>( new int[5]{ } );
	int *first = tst.get( );
	daw::not_null<int *> last = tst.get( ) + 5;
	daw_ensure( ( last - first ) == 5 );
	auto tst2 = std::unique_ptr<int[]>( new int[5]{ } );
	// lets test assignment
	first = tst2.get( );
	last = tst2.get( ) + 5;
	daw_ensure( ( last - first ) == 5 );
	return true;
}
static_assert( daw_not_null_test_004c( ) );
#endif
#endif

void daw_not_null_test_005( ) {
	auto tst = std::make_unique<int>( 5 );
	daw::not_null<int *> t = tst.get( );
	auto u = daw::not_null( daw::never_null, t );
	static_assert( std::is_same_v<DAW_TYPEOF( u )::pointer, int *> );
	(void)u;
}

void daw_not_null_test_006( ) {
	daw::not_null<std::optional<int>> t = { 5 };
	daw_ensure( t.get( ) == 5 );
}

int main( ) {
	daw_not_null_test_001( );
	daw_not_null_test_002( );
	daw_not_null_test_003( );
	daw_not_null_test_004( );
	daw_not_null_test_005( );
	daw_not_null_test_006( );
}

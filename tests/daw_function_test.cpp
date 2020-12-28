// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_function.h"

constexpr int func( int ) noexcept {
	return 1;
}
constexpr void func2( int ) noexcept {}

constexpr bool test_001( ) {
	auto call = []( int ) noexcept { return 0; };

	auto c0 = daw::make_callable( func );
	auto c1 = daw::make_callable( call );

	return c0( 5 ) + c1( 10 ) == 1;
}
static_assert( test_001( ) );

constexpr bool test_002( ) {
	auto call = []( int ) noexcept {};

	daw::make_callable( func2 );
	daw::make_callable( call );

	return true;
}
static_assert( test_002( ) );

int main( ) {}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_fnv1a_hash.h"

#include "daw/daw_benchmark.h"
#include "daw/daw_utility.h"

constexpr bool fnv1a_hash_test_001( ) {
	constexpr auto const c1 = daw::fnv1a_hash( "Hello" );
	constexpr auto const c2 = daw::fnv1a_hash( "Hello" );
	constexpr auto const c3 = daw::fnv1a_hash( 5 );
	constexpr auto const c4 = daw::fnv1a_hash( 0xFF00FF00FF00FF );
	Unused( c4 );
	daw::expecting( c1, c2 );
	daw::expecting( c1 != c3 );
	return true;
}
static_assert( fnv1a_hash_test_001( ) );

int main( ) {}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_generic_hash.h"

#include "daw/daw_benchmark.h"

constexpr bool test_01( ) {
	auto const c1 = daw::generic_hash( "Hello" );
	auto const c2 = daw::generic_hash( "Hello" );
	auto const c3 = daw::generic_hash( 5 );
	auto const c4 = daw::generic_hash( 0xFF00FF00FF00FF );
	daw::expecting( c1, c2 );
	daw::expecting( c1 != c3 );
	daw::expecting( c1 != c4 );
	return true;
}
static_assert( test_01( ) );

/*
constexpr bool test_16_01( ) {
  auto const c1 = daw::generic_hash<2>( "Hello" );
  auto const c2 = daw::generic_hash<2>( "Hello" );
  auto const c3 = daw::generic_hash<2>( 5 );
  auto const c4 = daw::generic_hash<2>( 0xFF00FF00FF00FF );
  daw::expecting( c1, c2 );
  daw::expecting( c1 != c3 );
  return true;
}
static_assert( test_16_01( ) );
*/

constexpr bool test_32_01( ) {
	auto const c1 = daw::generic_hash<4>( "Hello" );
	auto const c2 = daw::generic_hash<4>( "Hello" );
	auto const c3 = daw::generic_hash<4>( 5 );
	auto const c4 = daw::generic_hash<4>( 0xFF00FF00FF00FF );
	daw::expecting( c1, c2 );
	daw::expecting( c1 != c3 );
	daw::expecting( c1 != c4 );
	return true;
}
static_assert( test_32_01( ) );

constexpr bool test_64_01( ) {
	auto const c1 = daw::generic_hash<8>( "Hello" );
	auto const c2 = daw::generic_hash<8>( "Hello" );
	auto const c3 = daw::generic_hash<8>( 5 );
	auto const c4 = daw::generic_hash<8>( 0xFF00FF00FF00FF );
	daw::expecting( c1, c2 );
	daw::expecting( c1 != c3 );
	daw::expecting( c1 != c4 );
	return true;
}
static_assert( test_64_01( ) );

int main( ) {}

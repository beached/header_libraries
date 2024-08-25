// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/deprecated/daw_bounded_hash_set.h"

#include "daw/daw_benchmark.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_string_view.h"

constexpr bool test_002( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{ };
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	return true;
}
static_assert( test_002( ) );

constexpr bool test_004( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{ };
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		if( !adapt.exists( n ) ) {
			return false;
		}
	}
	return true;
}
static_assert( test_004( ) );

constexpr bool test_005( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{ };
	for( size_t n = 0; n < count; n += 2 ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		if( !adapt.exists( n ) and n % 2 == 0 ) {
			return false;
		}
	}
	return true;
}
static_assert( test_005( ) );

constexpr bool make_hash_set_001( ) {
	using namespace daw::string_view_literals;
	auto hs =
	  daw::make_bounded_hash_set<daw::string_view>( { "hello"_sv, "there"_sv } );
	return hs.count( "hello" ) != 0;
}
static_assert( make_hash_set_001( ) );

int main( ) {}

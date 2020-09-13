// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_bounded_vector.h"

constexpr bool daw_bounded_vector_test_001( ) {
	daw::bounded_vector_t<int, 10> a{ };
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	int sum = 0;
	for( auto c : a ) {
		sum += c;
	}
	daw::expecting( sum, 36 );
	return true;
}
static_assert( daw_bounded_vector_test_001( ) );

constexpr bool daw_bounded_vector_test_002( ) {
	daw::bounded_vector_t<int, 10> a{ };
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	daw::expecting( a.back( ), 4 );
	daw::expecting( a.pop_back( ), 4 );
	daw::expecting( a.back( ), 2 );
	daw::expecting( 2U, a.size( ) );
	daw::expecting( 10U, a.capacity( ) );
	daw::expecting( a.front( ), 1 );
	daw::expecting( *a.cbegin( ), 1 );
	daw::expecting( a.pop_back( ), 2 );
	daw::expecting( a.pop_back( ), 1 );
	daw::expecting( 0U, a.size( ) );
	daw::expecting( a.empty( ) );
	return true;
}
static_assert( daw_bounded_vector_test_002( ) );

constexpr bool daw_bounded_vector_test_003( ) {
	daw::bounded_vector_t<int, 6> a{ };
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	daw::expecting( a.full( ) );
	return true;
}
static_assert( daw_bounded_vector_test_003( ) );

namespace daw_bounded_vector_test_004_ns {
	constexpr auto cx_test( ) {
		daw::bounded_vector_t<int, 10> a{ };
		a.push_back( 1 );
		a.push_back( 2 );
		a.push_back( 4 );
		a.push_back( 8 );
		a.push_back( 16 );
		a.emplace_back( 5 );
		return a;
	}

	constexpr bool daw_bounded_vector_test_004( ) {
		constexpr auto a = cx_test( );
		int sum = 0;
		for( auto c : a ) {
			sum += c;
		}
		daw::expecting( sum, 36 );
		return true;
	}
	static_assert( daw_bounded_vector_test_004( ) );
} // namespace daw_bounded_vector_test_004_ns

int main( ) {}

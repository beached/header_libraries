// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_math.h"
#include "daw/daw_union_pair.h"

void daw_union_pair_test_001( ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	struct vis_t {
		constexpr int operator( )( int i ) const noexcept {
			return i;
		}
		constexpr int operator( )( double d ) const noexcept {
			return static_cast<int>( d );
		}
	};
	daw::expecting( 5, tmp.visit( vis_t( ) ) );
}

void daw_union_pair_test_002( ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	daw::expecting( 5, tmp.visit( []( auto v ) {
		return static_cast<int>( v );
	} ) );
}

void daw_union_pair_test_003( ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	tmp = 6.6;
	struct vis_t {
		constexpr double operator( )( int i ) const noexcept {
			return static_cast<double>( i );
		}
		constexpr double operator( )( double d ) const noexcept {
			return d;
		}
	};
	daw::expecting( daw::math::nearly_equal( 6.6, tmp.visit( vis_t( ) ), 0.1 ) );
}

void daw_union_pair_test_004( ) {
	auto tmp = daw::union_pair_t<int, double>( 5 );
	tmp = 6.6;
	daw::expecting( daw::math::nearly_equal( 6.6,
	                                         tmp.visit( []( auto v ) {
		                                         return static_cast<double>( v );
	                                         } ),
	                                         0.1 ) );
}

void daw_union_pair_test_005( ) {
	auto tmp = daw::union_pair_t<int, double>( );
	auto tmp2 = daw::union_pair_t<int, double>( 5 );
	tmp = tmp2;
}

int main( ) {
	daw_union_pair_test_001( );
	daw_union_pair_test_002( );
	daw_union_pair_test_003( );
	daw_union_pair_test_004( );
	daw_union_pair_test_005( );
}

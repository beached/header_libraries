// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_mutable_function_ref.h"

#include "daw/daw_ensure.h"

#include <cassert>
#include <concepts>
#include <cstdio>

int func( daw::mutable_function_ref<int( int, int, int )> f ) {
	return f( 1, 2, 3 ) * f( 4, 5, 7 );
}

int test( ) {
	return func( []( int a, int b, int c ) {
		return a * b * c;
	} );
}

inline constexpr int add( int a, int b, int c ) {
	return a + b + c;
}

int test2( ) {
	return func( add );
}

int func2( daw::mutable_function_ref<void( double, double )> f ) {
	f( 1.2, 3.4 );
	return 0;
}

int test3( ) {
	return func2( +[]( double, double ) {
		puts( "Hello\n" );
	} );
}

int call_mutable( daw::mutable_function_ref<int( )> f ) {
	return f( ) + f( );
}

int call_mutable2( daw::mutable_function_ref<int( )> const f ) {
	return f( ) + f( );
}

void call_mutable_void( daw::mutable_function_ref<void( )> f ) {
	f( );
	f( );
}

struct mutable_callable {
	int value = 0;

	int operator( )( ) {
		return ++value;
	}
};

static_assert( std::invocable<mutable_callable &> );
static_assert( not std::invocable<mutable_callable const &> );

int main( ) {
	auto const const_lvalue = []( ) {
		return 42;
	};
	daw::mutable_function_ref<int( )> const_view = const_lvalue;
	daw_ensure( const_view( ) == 42 );

	mutable_callable mutable_lvalue{ };
	daw_ensure( call_mutable( mutable_lvalue ) == 3 );
	daw_ensure( call_mutable( [n = 0]( ) mutable {
		            return ++n;
	            } ) == 3 );
	mutable_lvalue.value = 0;
	daw_ensure( call_mutable2( mutable_lvalue ) == 3 );
	daw_ensure( call_mutable2( [n = 0]( ) mutable {
		            return ++n;
	            } ) == 3 );

	int n = 0;
	auto mutable_void = [&n]( ) mutable {
		++n;
	};
	call_mutable_void( mutable_void );
	daw_ensure( n == 2 );

	struct FooCall {
		mutable int x = 0;

		constexpr void operator( )( ) {
			x = 1;
		}

		constexpr void operator( )( ) const {
			x = 2;
		}
	};

	{
		auto fc = FooCall{ };
		daw::mutable_function_ref<void( )> mfr_fc0 = fc;
		mfr_fc0( );
		daw_ensure( fc.x == 1 );
		daw::mutable_function_ref<void( )> mfr_fc1 = std::as_const( fc );
		mfr_fc1( );
		daw_ensure( fc.x == 2 );
	}
}

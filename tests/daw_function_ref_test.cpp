// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <cstdio>
#include <daw/daw_ensure.h>
#include <daw/daw_function_ref.h>

int func( daw::function_ref<int( int, int, int )> f ) {
	return f( 1, 2, 3 ) * f( 4, 5, 7 );
}

void test( ) {
	auto const r = func( []( int a, int b, int c ) {
		return a * b * c;
	} );
	daw_ensure( r == 840 );
}

inline constexpr int add( int a, int b, int c ) {
	return a + b + c;
}

void test2( ) {
	auto const r = func( add );
	daw_ensure( r == 96 );
}

void func2( daw::function_ref<void( double, double )> f ) {
	f( 1.2, 3.4 );
}

void test6( ) {
	int foo = 5;
	daw::function_ref<int( )> x = foo;
	daw_ensure( x( ) == 5 );
}

void test7( ) {
	int foo = 5;
	daw::function_ref<int( int )> x = foo;
	daw_ensure( x( 66 ) == 5 );
}

int main( ) {
	test( );
	test2( );
	test6( );
	test7( );
}

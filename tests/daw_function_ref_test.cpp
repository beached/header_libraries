// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_function_ref.h>

#include <daw/daw_consteval.h>
#include <daw/daw_ensure.h>

#include <cstdio>
#include <iostream>
#include <string>

constexpr int func( daw::function_ref<int( int, int, int )> f ) {
	return f( 1, 2, 3 ) * f( 4, 5, 7 );
}

DAW_CPP26_CONSTEVAL void test1( ) {
	constexpr auto l = []( int a, int b, int c ) {
		return a * b * c;
	};
	auto const r = func( l );
	daw_ensure( r == 840 );
	auto const r2 = func( +l );
	daw_ensure( r2 == 840 );
}

inline constexpr int add( int a, int b, int c ) {
	return a + b + c;
}

DAW_CONSTEVAL void test2( ) {
	auto const r = func( add );
	daw_ensure( r == 96 );
}

constexpr void func2( daw::function_ref<void( double, double )> f ) {
	f( 1.2, 3.4 );
}

DAW_CPP26_CONSTEVAL void test3( ) {
	auto fn = []( int a, std::string, double ) -> int {
		return a;
	};
	auto fnr = daw::function_ref<void( int, std::string, float )>( fn );
	fnr( 3, "Hello", 2.2f );
}

void test4( ) {
	auto fn = []( int a ) {
		std::cout << a << '\n';
	};

	auto fnr = daw::function_ref<void( int )>( fn );
	fnr( 1 );
	fnr = +fn;
	fnr( 2 );
}

int main( ) {
	test1( );
	test2( );
	test3( );
	test4( );
}

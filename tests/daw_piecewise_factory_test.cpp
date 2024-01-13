// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_piecewise_factory.h"

#include <exception>
#include <functional>
#include <string>
#include <utility>

struct A {
	int a = 0;
	int b = 0;
};

struct B : A {
	std::string c = "";
};

constexpr bool test( bool b ) noexcept {
	if( b ) {
		return true;
	} else {
		std::abort( );
	}
}

B test_001( ) noexcept {
	auto tmp = daw::piecewise_factory_late_t<B, int, int, std::string>( );
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	tmp.set<2>( []( ) {
		return "Hello";
	} );
	return tmp( );
}

constexpr A test_002( ) noexcept {
	auto tmp = daw::piecewise_factory_t<A, int, int>( );
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	return tmp( );
}
constexpr bool test_002_test( ) noexcept {
	A a{ 5, 6 };
	auto const tmp = test_002( );
	return a.a == tmp.a and a.b == tmp.b;
}
static_assert( test_002_test( ) );

B test_003( ) noexcept {
	auto tmp = daw::piecewise_factory_t<B, int, int, std::string>( );
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	tmp.set( 2, "Hello" );
	return tmp( );
}

constexpr A test_004( ) noexcept {
	auto tmp = daw::piecewise_factory_t<A, int, int>( );
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	return std::move( tmp )( );
}

constexpr bool test_004_test( ) noexcept {
	A a{ 5, 6 };
	auto const tmp = test_004( );
	return a.a == tmp.a and a.b == tmp.b;
}
static_assert( test_004_test( ) );

int main( ) {
	B c = test_001( );
	if( c.c != "Hello" ) {
		return 3;
	}
	B d = test_003( );
	if( d.c != "Hello" ) {
		return 1;
	}

	return 0;
}

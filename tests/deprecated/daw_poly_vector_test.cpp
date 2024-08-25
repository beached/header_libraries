// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/deprecated/daw_poly_vector.h"

struct A {
	int a{ };
	constexpr A( ) = default;
	constexpr A( A const & ) = default;
	constexpr A( A && ) = default;
	A &operator=( A const & ) = default;
	A &operator=( A && ) = default;
	virtual ~A( );
};
A::~A( ) = default;

struct B : public A {
	int b{ };
	B( ) = default;
	B( B const & ) = default;
	B( B && ) = default;
	B &operator=( B const & ) = default;
	B &operator=( B && ) = default;
	~B( ) override;
};
B::~B( ) = default;

void daw_poly_vector_01( ) {
	daw::poly_vector_t<A> test;
	test.push_back( B{ } );
}

int main( ) {
	daw_poly_vector_01( );
}

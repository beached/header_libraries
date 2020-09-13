// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_poly_vector.h"

struct A {
	int a;
	A( ) = default;
	A( A const & ) = default;
	A( A && ) = default;
	A &operator=( A const & ) = default;
	A &operator=( A && ) = default;
	virtual ~A( );
};
A::~A( ) {}

struct B : public A {
	int b;
	B( ) = default;
	B( B const & ) = default;
	B( B && ) = default;
	B &operator=( B const & ) = default;
	B &operator=( B && ) = default;
	~B( ) override;
};
B::~B( ) {}

void daw_poly_vector_01( ) {
	daw::poly_vector_t<A> test;
	test.push_back( B{ } );
}

int main( ) {
	daw_poly_vector_01( );
}

// The MIT License (MIT)
//
// Copyright (c) 2016-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/boost_test.h"
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

BOOST_AUTO_TEST_CASE( daw_poly_vector_01 ) {
	daw::poly_vector_t<A> test;
	test.push_back( B{} );
}

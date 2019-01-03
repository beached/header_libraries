// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <string>

#include "daw/daw_piecewise_factory.h"

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
		std::terminate( );
	}
}

B test_001( ) noexcept {
	auto tmp = daw::piecewise_factory_late_t<B, int, int, std::string>{};
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	tmp.set<2>( []( ) { return "Hello"; } );
	return tmp( );
}

constexpr A test_002( ) noexcept {
	auto tmp = daw::piecewise_factory_t<A, int, int>{};
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	return tmp( );
}
constexpr bool test_002_test( ) noexcept {
	A a{5, 6};
	auto const tmp = test_002( );
	return a.a == tmp.a and a.b == tmp.b;
}
static_assert( test_002_test( ) );

B test_003( ) noexcept {
	auto tmp = daw::piecewise_factory_t<B, int, int, std::string>{};
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	tmp.set( 2, "Hello" );
	return tmp( );
}

constexpr A test_004( ) noexcept {
	auto tmp = daw::piecewise_factory_t<A, int, int>{};
	tmp.set<0>( 5 );
	tmp.set( 1, 6 );
	return std::move( tmp )( );
}

constexpr bool test_004_test( ) noexcept {
	A a{5, 6};
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

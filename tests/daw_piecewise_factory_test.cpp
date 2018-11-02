// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include "daw/daw_piecewise_factory.h"

struct A {
	int a = 0;
	int b = 0;
};

constexpr bool test( bool b ) noexcept {
	if( b ) {
		return true;
	} else {
		std::terminate( );
	}
}

constexpr auto test_001( ) noexcept {
	auto tmp = daw::piecewise_factory_t<A, int, int>{};
	tmp.set<0>( 5 );
	tmp.set<1>( 6 );
	return tmp( );
}

int main( ) {
	auto a = test_001( );
	if( a.a != 5 ) {
		return 1;
	};
	if( a.b != 6 ) {
		return 2;
	};
	return 0;
}

// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_optional_poly.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

void daw_optional_poly_test_01( ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::optional_poly<int> ) -> "
	          << sizeof( daw::optional_poly<int> );
	std::cout << " sizeof( daw::optional_poly<size_t> ) -> "
	          << sizeof( daw::optional_poly<size_t> ) << '\n';
	daw::optional_poly<int> a{};
	daw::optional_poly<int> b{1};
	daw::optional_poly<int> c{2};
	daw::optional_poly<int> d{1};

	// a & b
	daw::expecting( !( a == b ) );
	daw::expecting( !( b == a ) );
	daw::expecting( a != b );
	daw::expecting( b != a );
	daw::expecting( a < b );
	daw::expecting( !( b < a ) );
	daw::expecting( a <= b );
	daw::expecting( !( b <= a ) );
	daw::expecting( !( a >= b ) );
	daw::expecting( b >= a );
	daw::expecting( b == d );
	daw::expecting( b != c );
	daw::expecting( b < c );
	daw::expecting( b <= c );
	daw::expecting( c > b );
	daw::expecting( c >= b );
}

void daw_optional_poly_test_02( ) {
	struct test_t {
		int8_t a;
	};

	daw::optional_poly<test_t> tmp = test_t{};
	tmp->a = 5;
}

void daw_optional_poly_test_03( ) {
	struct base_t {
		int blah;
		virtual ~base_t( ) = default;
		base_t( base_t const & ) = default;
		// base_t( base_t && ) noexcept = default;
		base_t &operator=( base_t const & ) = default;
		// base_t &operator=( base_t && ) noexcept = default;
		base_t( )
		  : blah{123} {}
		virtual int get( ) const {
			return blah;
		}
	};

	struct child_t : public base_t {
		int blah2;
		child_t( )
		  : blah2{321} {}
		explicit child_t( int x )
		  : blah2{x} {}
		int get( ) const override {
			return blah2;
		}
	};

	daw::optional_poly<base_t> a = base_t{};
	daw::optional_poly<base_t> b = child_t{};

	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	b = child_t{4543};
	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	daw::optional_poly<child_t> c = child_t{54321};
	a = c;
	std::cout << a->get( ) << " " << b->get( ) << " " << c->get( ) << std::endl;
}

int main( ) {
	daw_optional_poly_test_01( );
	daw_optional_poly_test_02( );
	daw_optional_poly_test_03( );
}

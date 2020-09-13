// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

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
	daw::optional_poly<int> a{ };
	daw::optional_poly<int> b{ 1 };
	daw::optional_poly<int> c{ 2 };
	daw::optional_poly<int> d{ 1 };

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

	daw::optional_poly<test_t> tmp = test_t{ };
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
		  : blah{ 123 } {}
		virtual int get( ) const {
			return blah;
		}
	};

	struct child_t : public base_t {
		int blah2;
		child_t( )
		  : blah2{ 321 } {}
		explicit child_t( int x )
		  : blah2{ x } {}
		int get( ) const override {
			return blah2;
		}
	};

	daw::optional_poly<base_t> a = base_t{ };
	daw::optional_poly<base_t> b = child_t{ };

	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	b = child_t{ 4543 };
	std::cout << a->get( ) << " " << b->get( ) << std::endl;
	daw::optional_poly<child_t> c = child_t{ 54321 };
	a = c;
	std::cout << a->get( ) << " " << b->get( ) << " " << c->get( ) << std::endl;
}

int main( ) {
	daw_optional_poly_test_01( );
	daw_optional_poly_test_02( );
	daw_optional_poly_test_03( );
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_checked_expected.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>

daw::checked_expected_t<int, std::runtime_error> divide( int v ) noexcept {
	try {
		if( v == 0 ) {
			throw std::runtime_error( "division by zero" );
		}
		return 4 / v;
	} catch( ... ) { return std::current_exception( ); }
}

daw::checked_expected_t<int, std::runtime_error> divide2( int v ) {
	try {
		if( v == 0 ) {
			throw std::runtime_error{ "division by zero" };
		}
		if( v > 4 ) {
			throw std::exception{ };
		}
		return 4 / v;
	} catch( ... ) { return std::current_exception( ); }
}

int divide3( int v ) {
	if( v == 0 ) {
		throw std::runtime_error{ "division by zero" };
	}
	if( v > 4 ) {
		throw std::exception{ };
	}
	return 4 / v;
}

void daw_expected_test_01( ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::checked_expected_t<int> ) -> "
	          << sizeof( daw::checked_expected_t<int> );
	std::cout << " sizeof( daw::checked_expected_t<size_t> ) -> "
	          << sizeof( daw::checked_expected_t<size_t> ) << '\n';
	daw::checked_expected_t<int> a{ };
	daw::checked_expected_t<int> b{ 1 };
	daw::checked_expected_t<int> c{ 2 };
	daw::checked_expected_t<int> d{ 1 };
	daw::checked_expected_t<void> e;
	auto X = []( int x ) {
		std::cout << "Hello\n";
		return x * x;
	};
	static_assert( daw::traits::is_callable_v<decltype( X ), int>,
	               "is_callable_v broke" );

	daw::checked_expected_t<void> f{ []( ) { std::cout << "Hello\n"; } };
	daw::checked_expected_t<void> g{ []( int ) { std::cout << "Hello\n"; }, 5 };
	// daw::checked_expected_t<int> h{ []( int x ) { std::cout << "Hello\n";
	// return x*x; }, 5 };

	auto h = divide( 0 );
	auto i = divide( 2 );
	auto h2 = divide2( 0 );
	try {
		auto i2 = divide2( 5 );
	} catch( std::runtime_error const & ) {
		throw std::runtime_error{ "This shouldn't happen" };
	} catch( ... ) {}

	try {
		int i3 = daw::checked_from_code<std::runtime_error>( divide3, 5 );
		Unused( i3 );
	} catch( std::runtime_error const & ) {
		throw std::runtime_error{ "This shouldn't happen" };
	} catch( ... ) {}

	daw::expecting( h.has_exception( ) );
	daw::expecting( !h.has_value( ) );
	daw::expecting( h2.has_exception( ) );
	daw::expecting( !h2.has_value( ) );
	daw::expecting( !i.has_exception( ) );
	daw::expecting( i.has_value( ) );
	// a & b
	auto test_01 = !( a == b );
	auto test_02 = !( b == a );

	daw::checked_expected_t<int, std::runtime_error> j{ &divide, 0 };
	daw::expecting( j.has_exception( ) );
	daw::expecting( !j.has_value( ) );

	daw::expecting( test_01 );
	daw::expecting( test_02 );

	struct L {
		int a;
	};

	daw::checked_expected_t<L> l{ []( ) { return L{ 5 }; } };
	daw::expecting( l->a == 5 );

	auto m = daw::make_checked_function<int, std::runtime_error>( []( int ii ) {
		if( ii == 0 ) {
			throw std::runtime_error( "Attempt to divide by zero" );
		} else if( ii > 100 ) {
			throw std::exception{ };
		}
		return 100 / ii;
	} );
	auto t = m( 0 );
	daw::expecting( t.has_exception( ) );
	daw::expecting( !t.has_value( ) );
	bool did_throw = false;
	try {
		m( 105 );
	} catch( ... ) { did_throw = true; }
	daw::expecting( did_throw );
}

int main( ) {
	daw_expected_test_01( );
}

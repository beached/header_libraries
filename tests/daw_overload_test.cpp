// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_overload.h"

#include <cstdint>
#include <iostream>
#include <system_error>

void daw_overload_001( ) {
	struct A {
		auto operator( )( int64_t ) const {
			std::cout << "A->int\n";
			return 0;
		}
	};
	struct B {
		auto operator( )( std::string const & ) const {
			std::cout << "B->std::string\n";
			return 1;
		}
	};
	struct C {
		auto operator( )( bool, bool ) const {
			std::cout << "D->bool\n";
			return 2;
		}
	};

	auto fn = daw::overload( A{ }, B{ }, C{ } );

	daw::expecting( fn( 1 ), 0 );
	daw::expecting( fn( std::string{ "Hello" } ), 1 );
	daw::expecting( fn( true, false ), 2 );

	auto const fn2 = daw::overload( A{ }, B{ }, C{ } );

	daw::expecting( fn2( 1 ), 0 );
	daw::expecting( fn2( std::string{ "Hello" } ), 1 );
	daw::expecting( fn2( true, false ), 2 );
}

void daw_overload_002( ) {
	auto ov = daw::overload(
	  []( int i ) {
		  return i * 2;
	  },
	  []( std::string const &s ) {
		  return s.size( );
	  } );
	auto const ov2 = daw::overload( ov, []( std::error_code ec ) {
		return ec.value( );
	} );

	daw::expecting( ov2( 1 ) == 2 );
	daw::expecting( ov2( "hi" ) == 2 );
	daw::expecting( ov2( std::error_code( ) ) == 0 );
}

void daw_empty_overload_001( ) {
	auto ov = daw::overload(
	  []( int i ) {
		  return i * 2;
	  },
	  []( std::string const &s ) {
		  return s.size( );
	  } );
	auto ov2 = daw::overload( ov, []( std::error_code ec ) {
		return ec.value( );
	} );

	daw::expecting( ov2( 1 ) == 2 );
	daw::expecting( ov2( "hi" ) == 2 );
	daw::expecting( ov2( std::error_code( ) ) == 0 );
	struct A {};
	bool const can_a = std::is_invocable_v<decltype( ov2 ), A>;
	daw::expecting( not can_a );
	// auto ov3 = daw::empty_overload<A>( ov2 );
	// bool const can_a2 = std::is_invocable_v<decltype( ov3 ), A>;
	// daw::expecting( can_a2 );
}

namespace daw_lift_overload_001 {
	int add( int a, int b ) {
		return a + b;
	}
	int add2( int a ) {
		return a;
	}

	void test( ) {
		auto add_set = daw::overload( daw::lift<add>, daw::lift<add2> );
		static_assert( sizeof( decltype( add_set ) ) == 1 );
		daw::expecting( add_set( 2, 4 ) == 6 );
		daw::expecting( add_set( 4 ) == 4 );
	}
} // namespace daw_lift_overload_001
int main( ) {
	daw_overload_001( );
	daw_overload_002( );
	daw_empty_overload_001( );
	daw_lift_overload_001::test( );
}

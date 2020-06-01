// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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
		auto operator( )( std::string ) const {
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

	auto fn = daw::overload( A{}, B{}, C{} );

	daw::expecting( fn( 1 ), 0 );
	daw::expecting( fn( std::string{"Hello"} ), 1 );
	daw::expecting( fn( true, false ), 2 );

	auto const fn2 = daw::overload( A{}, B{}, C{} );

	daw::expecting( fn2( 1 ), 0 );
	daw::expecting( fn2( std::string{"Hello"} ), 1 );
	daw::expecting( fn2( true, false ), 2 );
}

void daw_overload_002( ) {
	auto ov = daw::overload( []( int i ) { return i * 2; },
	                         []( std::string const &s ) { return s.size( ); } );
	auto const ov2 =
	  daw::overload( ov, []( std::error_code ec ) { return ec.value( ); } );

	daw::expecting( ov2( 1 ) == 2 );
	daw::expecting( ov2( "hi" ) == 2 );
	daw::expecting( ov2( std::error_code( ) ) == 0 );
}

void daw_empty_overload_001( ) {
	auto ov = daw::overload( []( int i ) { return i * 2; },
	                         []( std::string const &s ) { return s.size( ); } );
	auto ov2 =
	  daw::overload( ov, []( std::error_code ec ) { return ec.value( ); } );

	daw::expecting( ov2( 1 ) == 2 );
	daw::expecting( ov2( "hi" ) == 2 );
	daw::expecting( ov2( std::error_code( ) ) == 0 );
	struct A {};
	bool const can_a = daw::traits::is_callable_v<decltype( ov2 ), A>;
	daw::expecting( not can_a );
	// auto ov3 = daw::empty_overload<A>( ov2 );
	// bool const can_a2 = daw::traits::is_callable_v<decltype( ov3 ), A>;
	// daw::expecting( can_a2 );
}

int main( ) {
	daw_overload_001( );
	daw_overload_002( );
	daw_empty_overload_001( );
}

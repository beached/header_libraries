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
#include "daw/daw_expected.h"
#include "daw/daw_traits.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

daw::expected_t<int> divide( int v ) {
	try {
		if( 0 == v ) {
			throw std::runtime_error( "division by zero" );
		}
		return daw::expected_t<int>{4 / v};
	} catch( ... ) { return std::current_exception( ); }
}

void daw_expected_test_01( ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::expected_t<int> ) -> "
	          << sizeof( daw::expected_t<int> );
	std::cout << " sizeof( daw::expected_t<size_t> ) -> "
	          << sizeof( daw::expected_t<size_t> ) << '\n';
	daw::expected_t<int> a{};
	daw::expected_t<int> b{1};
	daw::expected_t<int> c{2};
	daw::expected_t<int> d{1};
	daw::expected_t<void> e;
	auto X = []( int x ) {
		std::cout << "Hello\n";
		return x * x;
	};
	static_assert( daw::traits::is_callable_v<decltype( X ), int>,
	               "is_callable_v broke" );

	daw::expected_t<void> f( []( ) { std::cout << "Hello\n"; } );
	daw::expected_t<void> g( []( int ) { std::cout << "Hello\n"; }, 5 );
	// daw::expected_t<int> h{ []( int x ) { std::cout << "Hello\n"; return x*x;
	// }, 5 };

	auto h = divide( 0 );
	auto i = divide( 2 );
	daw::expecting( h.has_exception( ) );
	daw::expecting( !h.has_value( ) );
	daw::expecting( !i.has_exception( ) );
	daw::expecting( i.has_value( ) );
	// a & b
	auto test_01 = !( a == b );
	auto test_02 = !( b == a );

	auto j = divide( 0 );
	daw::expecting( j.has_exception( ) );
	daw::expecting( !j.has_value( ) );

	daw::expecting( test_01 );
	daw::expecting( test_02 );

	auto const k = daw::expected_from_code( []( auto x ) { return x * x; }, 5 );
	daw::expecting( !k.empty( ) );
	daw::expecting( k.has_value( ) );
	daw::expecting( k.get( ) == 25 );
}

struct L {
	int a;
};

void daw_expected_test_02( ) {
	auto f = daw::expected_t<void>( []( ) { std::cout << "Hello\n"; } );

	auto l = daw::expected_t<L>( []( ) { return L{5}; }( ) );
	auto a = l->a == 5;
	daw::expecting( a );

	auto m = daw::expected_from_code( []( ) -> L { return L{6}; } );
	auto ma = m->a == 6;
	daw::expecting( ma );
}

void daw_expected_test_copy_001( ) {
	auto tmp = daw::expected_t<std::string>( );
	tmp = "This is a test";
	daw::expecting( tmp.has_value( ) );
	daw::expecting( !tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );
	daw::expecting( tmp.get( ) == "This is a test" );
	auto tmp2 = daw::expected_t<std::string>( );
	tmp2 = tmp;
	daw::expecting( tmp.has_value( ) == tmp2.has_value( ) );
	daw::expecting( tmp.has_exception( ) == tmp2.has_exception( ) );
	daw::expecting( tmp.empty( ) == tmp2.empty( ) );
	daw::expecting( tmp.get( ) == tmp2.get( ) );

	auto tmp3 = daw::expected_t<std::string>( tmp );
	daw::expecting( tmp.has_value( ) == tmp3.has_value( ) );
	daw::expecting( tmp.has_exception( ) == tmp3.has_exception( ) );
	daw::expecting( tmp.empty( ) == tmp3.empty( ) );
	daw::expecting( tmp.get( ) == tmp3.get( ) );
}

void daw_expected_test_copy_002( ) {
	auto tmp = daw::expected_t<std::string>( );
	try {
		throw std::exception{};
	} catch( ... ) { tmp.set_exception( ); }
	daw::expecting( !tmp.has_value( ) );
	daw::expecting( tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );

	daw::expecting_exception( [&]( ) { tmp.get( ); } );

	auto tmp2 = daw::expected_t<std::string>( );
	tmp2 = tmp;
	daw::expecting( tmp.has_value( ) == tmp2.has_value( ) );
	daw::expecting( tmp.has_exception( ) == tmp2.has_exception( ) );
	daw::expecting( tmp.empty( ) == tmp2.empty( ) );

	daw::expecting_exception( [&]( ) { tmp2.get( ); } );

	auto tmp3 = daw::expected_t<std::string>( tmp );
	daw::expecting( tmp.has_value( ) == tmp3.has_value( ) );
	daw::expecting( tmp.has_exception( ) == tmp3.has_exception( ) );
	daw::expecting( tmp.empty( ) == tmp3.empty( ) );
	daw::expecting_exception( [&]( ) { tmp3.get( ); } );
}

void daw_expected_test_move_assignment_001( ) {
	auto tmp = daw::expected_t<std::string>( );
	tmp = "This is a test";
	daw::expecting( tmp.has_value( ) );
	daw::expecting( !tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );
	daw::expecting( tmp.get( ) == "This is a test" );
	auto tmp2 = daw::expected_t<std::string>( );
	tmp2 = std::move( tmp );
	daw::expecting( tmp2.has_value( ) );
	daw::expecting( !tmp2.has_exception( ) );
	daw::expecting( !tmp2.empty( ) );
	daw::expecting( tmp2.get( ) == "This is a test" );
}

void daw_expected_test_move_construction_001( ) {
	auto tmp = daw::expected_t<std::string>( );
	tmp = "This is a test";
	daw::expecting( tmp.has_value( ) );
	daw::expecting( !tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );
	daw::expecting( tmp.get( ) == "This is a test" );
	auto tmp2 = daw::expected_t<std::string>( std::move( tmp ) );
	daw::expecting( tmp2.has_value( ) );
	daw::expecting( !tmp2.has_exception( ) );
	daw::expecting( !tmp2.empty( ) );
	daw::expecting( tmp2.get( ) == "This is a test" );
}

void daw_expected_test_move_assignment_002( ) {
	auto tmp = daw::expected_t<std::string>( );
	try {
		throw std::exception{};
	} catch( ... ) { tmp.set_exception( ); }
	daw::expecting( !tmp.has_value( ) );
	daw::expecting( tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );
	daw::expecting_exception( [&]( ) { tmp.get( ); } );

	auto tmp2 = daw::expected_t<std::string>( );
	tmp2 = std::move( tmp );
	daw::expecting( !tmp2.has_value( ) );
	daw::expecting( tmp2.has_exception( ) );
	daw::expecting( !tmp2.empty( ) );
	daw::expecting_exception( [&]( ) { tmp2.get( ); } );
}

void daw_expected_test_move_construction_002( ) {
	auto tmp = daw::expected_t<std::string>( );
	try {
		throw std::exception{};
	} catch( ... ) { tmp.set_exception( ); }
	daw::expecting( !tmp.has_value( ) );
	daw::expecting( tmp.has_exception( ) );
	daw::expecting( !tmp.empty( ) );
	daw::expecting_exception( [&]( ) { tmp.get( ); } );

	auto tmp2 = daw::expected_t<std::string>( std::move( tmp ) );
	daw::expecting( !tmp2.has_value( ) );
	daw::expecting( tmp2.has_exception( ) );
	daw::expecting( !tmp2.empty( ) );
	daw::expecting_exception( [&]( ) { tmp2.get( ); } );
}

int main( ) {
	daw_expected_test_01( );
	daw_expected_test_02( );
	daw_expected_test_copy_001( );
	daw_expected_test_copy_002( );
	daw_expected_test_move_assignment_001( );
	daw_expected_test_move_construction_001( );
	daw_expected_test_move_assignment_002( );
	daw_expected_test_move_construction_002( );
}

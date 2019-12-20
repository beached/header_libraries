// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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
#include "daw/daw_optional.h"

#include <cstddef>
#include <iostream>

void daw_optional_test_01( ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::optional<int> ) -> "
	          << sizeof( daw::optional<int> );
	std::cout << " sizeof( daw::optional<size_t> ) -> "
	          << sizeof( daw::optional<size_t> ) << '\n';
	constexpr auto a = daw::optional<int>( );
	constexpr auto b = daw::optional<int>( 1 );
	constexpr auto c = daw::optional<int>( 2 );
	constexpr auto d = daw::optional<int>( 1 );

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

void daw_optional_test_02( ) {
	auto a = ::daw::optional<int>( );
	daw::expecting( not a );
	a = 5;
	daw::expecting( a );
	daw::expecting( a, 5 );
}

void daw_optional_test_ref_01( ) {
	auto a = ::daw::optional<int &>( );
	int x = 5;
	daw::expecting( not a );
	a = ::daw::optional<int &>( x );
	daw::expecting( a );
	daw::expecting( a, 5 );
	*a = 6;
	daw::expecting( a, 6 );
	daw::expecting( x, 6 );
}

void daw_optional_test_ref_02( ) {
	auto a = ::daw::optional<int const &>( );
	int x = 5;
	daw::expecting( not a );
	a = ::daw::optional<int const &>( x );
	daw::expecting( a );
	daw::expecting( a, 5 );
	int y = 6;
	a = ::daw::optional<int const &>( y );
	daw::expecting( a );
	daw::expecting( a, 6 );
}

int main( ) {
	daw_optional_test_01( );
	daw_optional_test_02( );
	daw_optional_test_ref_01( );
	daw_optional_test_ref_02( );
}

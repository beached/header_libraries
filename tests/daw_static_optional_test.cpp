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

#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/daw_static_optional.h"

void daw_optional_test_01( ) {
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
	std::cout << " sizeof( int ) -> " << sizeof( int );
	std::cout << " sizeof( daw::static_optional<int> ) -> "
	          << sizeof( daw::static_optional<int> );
	std::cout << " sizeof( daw::static_optional<size_t> ) -> "
	          << sizeof( daw::static_optional<size_t> ) << '\n';
	constexpr daw::static_optional<int> a{};
	constexpr daw::static_optional<int> b{1};
	constexpr daw::static_optional<int> c{2};

	static_assert( !a, "" );
	// a & b
	static_assert( !( a == b ), "" );
	static_assert( !( b == a ), "" );
	static_assert( a != b, "" );
	static_assert( b != a, "" );
	static_assert( a < b, "" );
	static_assert( !( b < a ), "" );
	static_assert( a <= b, "" );
	static_assert( !( b <= a ), "" );
	static_assert( !( a >= b ), "" );
	static_assert( b >= a, "" );
	static_assert( b != c, "" );
	static_assert( b < c, "" );
	static_assert( b <= c, "" );
	static_assert( c > b, "" );
	static_assert( c >= b, "" );

	daw::static_optional<int> d{1};
	d = 2;
	d = 1;
	daw::expecting( b, d );
}

int main( ) {
	daw_optional_test_01( );
}

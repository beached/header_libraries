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
#include "daw/daw_heap_value.h"

#include <cstdint>
#include <iostream>

void daw_heap_value_test_01( ) {
	struct lrg {
		size_t a;
		size_t b;
		size_t c;
		size_t d;
		size_t e;
	};
	std::cout << "sizeof( int ) -> " << sizeof( int ) << '\t';
	std::cout << "sizeof( daw::heap_value<int> ) -> "
	          << sizeof( daw::heap_value<int> ) << '\n';
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t ) << '\t';
	std::cout << "sizeof( daw::heap_value<size_t> ) -> "
	          << sizeof( daw::heap_value<size_t> ) << '\n';
	std::cout << "sizeof( lrg ) -> " << sizeof( lrg ) << '\t';
	std::cout << "sizeof( daw::heap_value<lrg> ) -> "
	          << sizeof( daw::heap_value<lrg> ) << '\n';
	daw::heap_value<int> a{};
	daw::heap_value<int> b{1};
	daw::heap_value<int> c{2};
	daw::heap_value<int> d{1};

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

int main( ) {
	daw_heap_value_test_01( );
}

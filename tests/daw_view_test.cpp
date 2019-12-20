// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_view.h"

#include <array>
#include <functional>

constexpr bool test_const_iterator_01( ) {
	using arry_t = std::array<int, 5>;
	arry_t arry = {1, 2, 3, 4, 5};
	daw::view a( arry.begin( ), arry.end( ) );
	daw::expecting( a.size( ) == 5 );
	daw::expecting( *arry.begin( ) == 1 );
	daw::expecting( a.size( ), arry.size( ) );

	daw::expecting(
	  daw::algorithm::equal( arry.begin( ), arry.end( ), a.begin( ), a.end( ) ) );

	return true;
}

static_assert( test_const_iterator_01( ) );

int main( ) {
	test_const_iterator_01( );
	return 0;
}

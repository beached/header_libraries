// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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
#include "daw/iterator/daw_reverse_iterator.h"

#include <array>

constexpr bool reverse_iterator_test_001( ) {
	std::array<int, 6> a = {1, 2, 3, 4, 5, 6};
	std::array<int, 6> b{};
	auto first = daw::make_reverse_iterator( a.end( ) );
	auto last = daw::make_reverse_iterator( a.begin( ) );
	auto first_out = daw::make_reverse_iterator( b.end( ) );
	daw::algorithm::copy( first, last, first_out );
	return daw::algorithm::equal( a.begin( ), a.end( ), b.begin( ) );
}
static_assert( reverse_iterator_test_001( ) );

int main( ) {}

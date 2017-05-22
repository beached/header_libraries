// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw_memory_address.h"
#include <boost/test/unit_test.hpp>
#include <cstdint>

int test( int b ) {
	return 2 * b;
}

BOOST_AUTO_TEST_CASE( daw_memory_address_test_01 ) {
	daw::memory_address<int, 0x1> a;
	daw::memory_address<int, 0x1, sizeof( int ) * 1> b;
	daw::memory_address<int, 0x1, sizeof( int ) * 2> c;
	daw::memory_address<int, 0x1, sizeof( int ) * 3> d;
	auto e = a + b + c + d;
	auto f = test( b );
	bool t = a == b;
}

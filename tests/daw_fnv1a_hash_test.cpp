// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include "boost_test.h"
#include <iostream>

#include "daw_fnv1a_hash.h"

BOOST_AUTO_TEST_CASE( test_01 ) {
	constexpr auto const c1 = daw::fnv1a_hash( "Hello" );
	constexpr auto const c2 = daw::fnv1a_hash( "Hello" );
	constexpr auto const c3 = daw::fnv1a_hash( 5 );
	constexpr auto const c4 = daw::fnv1a_hash( 0xFF00FF00FF00FF );
	std::cout << c1 << " " << c2 << " " << c3 << " " << c4 << '\n';
	BOOST_REQUIRE_EQUAL( c1, c2 );
	BOOST_REQUIRE( c1 != c3 );
}

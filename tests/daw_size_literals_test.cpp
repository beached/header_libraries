// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include "boost_test.h"
#include "daw_size_literals.h"

BOOST_AUTO_TEST_CASE( daw_size_literals_001 ) {
	using namespace daw::size_literals;
	BOOST_REQUIRE_EQUAL( 1_KB, 1024ull );
	BOOST_REQUIRE_EQUAL( 1_MB, 1024ull * 1024u );
	BOOST_REQUIRE_EQUAL( 1_GB, 1024ull * 1024u * 1024u );
	BOOST_REQUIRE_EQUAL( 1_TB, 1024ull * 1024u * 1024u * 1024u );
	BOOST_REQUIRE_EQUAL( 1_PB, 1024ull * 1024u * 1024u * 1024u * 1024u );
}

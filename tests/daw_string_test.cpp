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

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw/boost_test.h"
#include "daw/daw_string.h"

BOOST_AUTO_TEST_CASE( make_string_testing ) {
	std::string a = "a b c d e f";
	auto b = daw::string::split_if( a, &isspace );

	BOOST_REQUIRE_EQUAL( b.size( ), 6 );
}

BOOST_AUTO_TEST_CASE( string_fmt_test_001 ) {
	auto result = daw::string::fmt( "This is {0} test", "a" );
	BOOST_REQUIRE_EQUAL( result, "This is a test" );
}

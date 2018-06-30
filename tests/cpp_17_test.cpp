// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights home/dwright/github/lib_nodepp/ to use, copy, modify, merge, publish,
// distribute, sublicense, and / or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the following
// conditions:
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

#include "daw/boost_test.h"
#include <iostream>

#include "daw/cpp_17.h"

namespace void_t_tests {
	void test2( int ) {}
	struct X {};

	bool fn2( ... ) {
		return true;
	}

	template<typename T, typename = daw::void_t<decltype( fn( T{} ) )>>
	bool fn2( T ) {
		return false;
	}

	BOOST_AUTO_TEST_CASE( void_t_test_001 ) {
		BOOST_REQUIRE( fn2( X{} ) );
	}
} // namespace void_t_tests

void test( ) {}
BOOST_AUTO_TEST_CASE( cpp_17_test_01 ) {
	constexpr auto const is_func = daw::is_function_v<decltype( test )>;
	constexpr auto const isnt_func = daw::is_function_v<decltype( is_func )>;

	BOOST_REQUIRE( is_func );
	BOOST_REQUIRE( !isnt_func );
}

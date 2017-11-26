// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#include "boost_test.h"
#include "daw_utility.h"
#include <sstream>

BOOST_AUTO_TEST_CASE( daw_utility_append_test ) {
	std::vector<int> a{1, 2, 3, 4, 5};
	daw::append( a, 6, 7, 8, 9 );
	BOOST_REQUIRE( a.size( ) == 9 );
}

BOOST_AUTO_TEST_CASE( daw_hex_test_001 ) {
	uint32_t const a = 0xFF00FFFF;
	std::string a_str;
	daw::hex( a, std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
	BOOST_REQUIRE_EQUAL( a_str, "FF00FFFF" );
}

BOOST_AUTO_TEST_CASE( daw_hex_test_002 ) {
	std::vector<uint32_t> vec_a( 10, 0xFF00FFFF );
	std::string a_str;
	daw::hex( vec_a.cbegin( ), vec_a.cend( ), std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
}

BOOST_AUTO_TEST_CASE( daw_hex_test_003 ) {
	std::string a_str;
	daw::hex( "abcdef", std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
}

constexpr bool to_hex_test_004( ) noexcept {
	uint32_t const a = 0xFF00FFFF;
	char result[9] = {0};
	char const expected[] = "FF00FFFF";
	char *ptr = result;
	daw::hex( a, ptr );
	for( size_t n = 0; n < 8; ++n ) {
		if( result[n] != expected[n] ) {
			return false;
		}
	}
	return true;
}

BOOST_AUTO_TEST_CASE( daw_hex_test_004 ) {
	BOOST_REQUIRE( to_hex_test_004( ) );
}

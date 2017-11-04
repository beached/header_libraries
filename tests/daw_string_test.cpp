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

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "boost_test.h"
#include "daw_string.h"

BOOST_AUTO_TEST_CASE( make_string_testing ) {
	std::string a = "a b c d e f";
	auto b = daw::string::split_if( a, &isspace );

	BOOST_REQUIRE_EQUAL( b.size( ), 6 );
}

template<typename string_t>
void test_string_view( ) {
	constexpr bool has_integer_subscript = daw::traits::has_integer_subscript_v<string_t>;
	constexpr bool has_size_memberfn = daw::traits::has_size_memberfn_v<string_t>;
	constexpr bool has_empty_memberfn = daw::traits::has_empty_memberfn_v<string_t>;
	constexpr bool is_not_array_array = daw::traits::is_not_array_array_v<string_t>;
	constexpr bool is_string_view_like = daw::traits::is_string_view_like_v<string_t>;

	BOOST_REQUIRE( has_integer_subscript );
	BOOST_REQUIRE( has_size_memberfn );
	BOOST_REQUIRE( has_empty_memberfn );
	BOOST_REQUIRE( is_not_array_array );
	BOOST_REQUIRE( is_string_view_like );
}

template<typename string_t>
void test_string( ) {
	test_string_view<string_t>( );
	constexpr bool has_append_operator = daw::traits::has_append_operator_v<string_t>;
	constexpr bool has_append_memberfn = daw::traits::has_append_memberfn_v<string_t>;
	constexpr bool is_string_like = daw::traits::is_string_like_v<string_t>;

	BOOST_REQUIRE( has_append_operator );
	BOOST_REQUIRE( has_append_memberfn );
	BOOST_REQUIRE( is_string_like );
}


BOOST_AUTO_TEST_CASE( string_concept_test_001 ) {
		test_string<std::string>( );
}

BOOST_AUTO_TEST_CASE( string_view_concept_test_001 ) {
		test_string_view<daw::string_view>( );
}

BOOST_AUTO_TEST_CASE( string_view_concept_test_002 ) {
		test_string_view<std::string>( );
}

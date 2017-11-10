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
#include "daw_parse_to.h"

BOOST_AUTO_TEST_CASE( daw_values_from_stream_001 ) {
	constexpr auto vals = daw::parser::parse_to<int, int, int, int>( "0,1,2,3", "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<1>( vals ), 1 );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}

BOOST_AUTO_TEST_CASE( daw_values_from_stream_002 ) {
	auto vals = daw::parser::parse_to<int, std::string, int, int>( "0,\"hello there\",2,3", "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<std::string>( vals ), "hello there" );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}

BOOST_AUTO_TEST_CASE( daw_values_from_stream_003 ) {
	constexpr daw::string_view const str = R"(0,"hello there",2,3)";
	constexpr auto vals = daw::parser::parse_to<int, daw::string_view, int, int>( str, "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<daw::string_view>( vals ), "hello there" );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}


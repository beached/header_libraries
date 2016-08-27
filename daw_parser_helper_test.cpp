// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>

#include "daw_parser_helper.h"

BOOST_AUTO_TEST_CASE( daw_parser_helper001 ) {
	static std::string uint_test = "43453";
	uint32_t ui32 = 0;
	int32_t i32 = 0;
	daw::parser::to_uint( uint_test.begin( ), uint_test.end( ), ui32 );
	std::cout << "to_uint: uint32 parser test from: '" << uint_test << "' to " << ui32 << '\n';
	BOOST_REQUIRE_MESSAGE( 43453 == ui32, "Value did not parse correctly" );
	std::cout << std::endl;
	daw::parser::to_int( uint_test.begin( ), uint_test.end( ), i32 );
	std::cout << "to_int: int32 parser test from: '" << uint_test << "' to " << i32 << '\n';
	BOOST_REQUIRE_MESSAGE( 43453 == i32, "Value did not parse correctly" );

	static std::string int_test = "-34534";
	i32 = 0;
	daw::parser::to_int( int_test.begin( ), int_test.end( ), i32 );
	std::cout << "to_int: int32 parser test from: '" << int_test << "' to " << i32 << '\n';
	BOOST_REQUIRE_MESSAGE( -34534 == i32, "Value did not parse correctly" );

	static std::string tr_test = "  hello  ";
	std::cout << "Test trimming '" << tr_test << "'\n";

	auto tr_left = daw::parser::trim_left( tr_test.begin( ), tr_test.end( ) ).as_string( );
	std::cout << "Left only '" << tr_left << "'\n";
	BOOST_REQUIRE_MESSAGE( tr_left == "hello  ", "Trim left did not work" );

	auto tr_right = daw::parser::trim_right( tr_test.begin( ), tr_test.end( ) ).as_string( );
	std::cout << "Right only '" << tr_right << "'\n";
	BOOST_REQUIRE_MESSAGE( tr_right == "  hello", "Trim right did not work" );

	auto tr_full = daw::parser::trim( tr_test.begin( ), tr_test.end( ) ).as_string( );
	std::cout << "Full trim '" << tr_full << "'\n";
	BOOST_REQUIRE_MESSAGE( tr_full == "hello", "Trim did not work" );
	
	static std::string empty;
	auto empty_left = daw::parser::trim_left( empty.begin( ), empty.end( ) ).as_string( );
	std::cout << "Empty Left only '" << empty_left << "'\n";
	BOOST_REQUIRE_MESSAGE( empty_left == "", "Trim left did not work" );

	auto empty_right = daw::parser::trim_right( empty.begin( ), empty.end( ) ).as_string( );
	std::cout << "Empty Right only '" << empty_right << "'\n";
	BOOST_REQUIRE_MESSAGE( empty_right == "", "Trim right did not work" );

	auto empty_full = daw::parser::trim( empty.begin( ), empty.end( ) ).as_string( );
	std::cout << "Empty full trim '" << empty_full << "'\n";
	BOOST_REQUIRE_MESSAGE( empty_full == "", "Trim did not work" );

}


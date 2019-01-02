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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_container_algorithm.h"
#include "daw/daw_string_split_range.h"

BOOST_AUTO_TEST_CASE( string_split_range_001 ) {
	std::string const str = "This is a test of the split";
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};

	auto rng = daw::split_string( str, " " );
	daw::algorithm::transform( rng.begin( ), rng.end( ),
	                           std::back_inserter( tst ),
	                           []( auto sv ) { return sv.to_string( ); } );

	bool const ans = std::equal( tst.cbegin( ), tst.cend( ),
	                             expected_tst.cbegin( ), expected_tst.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( string_split_range_002 ) {
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};

	for( auto sv : daw::split_string( "This is a test of the split", " " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal( tst.cbegin( ), tst.cend( ),
	                             expected_tst.cbegin( ), expected_tst.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( string_split_range_003 ) {
	std::vector<std::string> const expected_tst = {"This", "is",  "a",    "test",
	                                               "of",   "the", "split"};
	std::vector<std::string> tst{};

	for( auto sv :
	     daw::split_string( "This  is  a  test  of  the  split", "  " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal( tst.cbegin( ), tst.cend( ),
	                             expected_tst.cbegin( ), expected_tst.cend( ) );
	BOOST_REQUIRE( ans );
}

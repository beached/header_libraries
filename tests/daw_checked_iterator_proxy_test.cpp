// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
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

#include "daw_checked_iterator_proxy.h"

BOOST_AUTO_TEST_CASE( daw_checked_iterator_proxy_001 ) {
	std::vector<int> const test = {0, 1, 2, 3, 4};

	for( auto it = daw::make_checked_iterator_proxy( test.begin( ), test.end( ) ); it != test.end( ); ++it ) {
		std::cout << *it << '\n';
	}
}

BOOST_AUTO_TEST_CASE( daw_checked_iterator_proxy_002 ) {
	std::vector<int> const test = {0, 1, 2, 3, 4};

	bool result = false;
	try {
		for( auto it = daw::make_checked_iterator_proxy( test.begin( ), test.begin( ) + 2 ); it != test.end( ); ++it ) {
			std::cout << *it << '\n';
		}
	} catch( std::out_of_range const &ex ) {
		result = true;
		std::cout << "Expected exception with message: " << ex.what( ) << '\n';
	}
	BOOST_REQUIRE_MESSAGE( result, "Expected an out_of_range exception but didn't get one" );
}

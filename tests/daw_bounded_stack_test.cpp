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
#include "daw_bounded_stack.h"

BOOST_AUTO_TEST_CASE( daw_bounded_stack_testing ) {
	daw::bounded_stack_t<int, 5> tst;
	BOOST_REQUIRE( tst.empty( ) );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 1 );
	BOOST_REQUIRE( tst.back( ) == 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.full( ) );
	BOOST_REQUIRE( tst.pop_back( ) == 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';

	BOOST_REQUIRE( tst.pop_back( ) == 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop_back( ) == 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop_back( ) == 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop_back( ) == 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.empty( ) );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push_back( 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.full( ) );

	BOOST_REQUIRE( !tst.empty( ) );
	tst.clear( );
	BOOST_REQUIRE( tst.empty( ) );
}

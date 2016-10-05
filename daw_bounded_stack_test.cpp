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
	tst.push( 1 );
	BOOST_REQUIRE( tst.peek( ) == 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.full( ) );
	BOOST_REQUIRE( tst.pop( ) == 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';

	BOOST_REQUIRE( tst.pop( ) == 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop( ) == 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop( ) == 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.pop( ) == 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.empty( ) );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 1 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 2 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 3 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 4 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	tst.push( 5 );
	std::cout << "Used: " << tst.used( ) << " Avail: " << tst.available( ) << '\n';
	BOOST_REQUIRE( tst.full( ) );
	BOOST_REQUIRE( tst.pop( ) == 5 );

}

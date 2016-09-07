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
#include <tuple>

#include "daw_tuple_helper.h"

using namespace daw::tuple::operators;
using namespace daw::tuple;

BOOST_AUTO_TEST_CASE( daw_tuple_math001 ) {
	auto t1 = std::make_tuple( 1.0, 2.0, 3.0 );
	auto res1 = t1 * 2.0;
	auto res2 = 2.0 * t1;
	std::cout << "result of 2*" << t1 << " => " << res1 << std::endl;
	std::cout << "result of " << t1 << "*2 => " << res2 << std::endl;

	auto t2 = std::make_tuple( 4.0, 2.0, 1.0 );
	auto res3 = t1/t2;
	auto res6 = 1.0 + t1;
	auto res7 = t1 + 1.0;
	auto res8 = 1.0/t1;
	auto res9 = t1/2.0;
	auto res10 = 1.0 - t1;
	auto res11 = t1 - 1.0;
	auto res12 = 2.0 * t1;
	auto res13 = t1 * 2.0;
	auto res14 = t1 + t2;
	auto res15 = t1 - t2;
	auto res16 = t1 * t2;

	std::cout << "result of 1.0 + " << t1 << " => " << res6 << std::endl;
	std::cout << "result of " << t1 << " + 2.0 => " << res7 << std::endl;

	std::cout << "result of 1.0 - " << t1 << " => " << res10 << std::endl;
	std::cout << "result of " << t1 << " - 1.0 => " << res11 << std::endl;

	std::cout << "result of 2.0 * " << t1 << " => " << res12 << std::endl;
	std::cout << "result of " << t1 << " * 2.0 => " << res13 << std::endl;

	std::cout << "result of 1.0 / " << t1 << " => " << res8 << std::endl;
	std::cout << "result of " << t1 << " / 2.0 => " << res9 << std::endl;

	std::cout << "result of " << t1 << " + " << t2 << " => " << res14 << std::endl;
	std::cout << "result of " << t1 << " - " << t2 << " => " << res15 << std::endl;
	std::cout << "result of " << t1 << " * " << t2 << " => " << res16 << std::endl;
	std::cout << "result of " << t1 << " / " << t2 << " => " << res3 << std::endl;

	BOOST_REQUIRE( res1 == res2 );
	BOOST_REQUIRE( res3 == std::make_tuple( 0.25, 1.0, 3.0 ) ); 
	BOOST_REQUIRE( res6 == std::make_tuple( 2.0, 3.0, 4.0 ) ); 
	BOOST_REQUIRE( res7 == std::make_tuple( 2.0, 3.0, 4.0 ) ); 
	BOOST_REQUIRE( res8 == std::make_tuple( 1.0, 0.5, 1.0/3.0 ) ); 
	BOOST_REQUIRE( res9 == std::make_tuple( 0.5, 1.0, 1.5 ) ); 
	BOOST_REQUIRE( res10 == std::make_tuple( 0.0, -1.0, -2.0 ) ); 
	BOOST_REQUIRE( res11 == std::make_tuple( 0.0, 1.0, 2.0 ) ); 
	BOOST_REQUIRE( res12 == std::make_tuple( 2.0, 4.0, 6.0 ) ); 
	BOOST_REQUIRE( res13 == std::make_tuple( 2.0, 4.0, 6.0 ) ); 
	BOOST_REQUIRE( res14 == std::make_tuple( 5.0, 4.0, 4.0 ) ); 
	BOOST_REQUIRE( res15 == std::make_tuple( -3.0, 0.0, 2.0 ) ); 
	BOOST_REQUIRE( res16 == std::make_tuple( 4.0, 4.0, 3.0 ) ); 
	BOOST_REQUIRE( res3 == std::make_tuple( 0.25, 1.0, 3.0 ) ); 
}


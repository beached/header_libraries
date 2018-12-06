// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
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

#include <iostream>

#include "daw/boost_test.h"
#include "daw/daw_benchmark.h"

BOOST_AUTO_TEST_CASE( daw_benchmark_test_001 ) {
	std::cout << "Time of: "
	          << daw::benchmark( []( ) { std::cout << "Hello\n"; } );
	daw::show_benchmark( 1, "timed", []( ) { std::cout << "Hello\n"; } );
}

BOOST_AUTO_TEST_CASE( daw_benchmark_test_002 ) {
	int x = 0;
	daw::do_not_optimize( x );
}

BOOST_AUTO_TEST_CASE( daw_bench_test_test_001 ) {
	auto res = daw::bench_test( "sqr: ", []( auto i ) { return i * i; }, 55 );
	BOOST_REQUIRE( res.has_value( ) );
	BOOST_REQUIRE_EQUAL( *res, 3025 );
}

BOOST_AUTO_TEST_CASE( daw_bench_n_test_test_001 ) {
	auto res = daw::bench_n_test<100>( "sqr: ", []( auto i ) { return i * i; }, 55 );
	BOOST_REQUIRE( res.has_value( ) );
	BOOST_REQUIRE_EQUAL( *res, 3025 );
}

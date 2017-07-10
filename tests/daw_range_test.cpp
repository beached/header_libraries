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
#include "daw_range.h"

#ifndef WIN32 // Fails on MSVC 2015.2
BOOST_AUTO_TEST_CASE( daw_range_test01 ) {
	using namespace daw::range::operators;
	using daw::range::from;

	std::vector<int32_t> const t = {-400, 4, -1, 1000, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto result = from( t ) << shuffle( ) << sort( ) << where( []( auto v ) { return v > 0; } )
	                        << stable_partition( []( auto v ) { return v % 2 == 0; } );
	std::cout << "Test 001\n";
	std::cout << result << std::endl;

	std::cout << "{";
	for( auto const &v : result ) {
		std::cout << " " << v;
	}
	std::cout << " }\n";
}
#endif // WIN32

BOOST_AUTO_TEST_CASE( daw_range_test02 ) {
	using daw::range::from;

	std::vector<int32_t> const t = {-400, 4, -1, 1000, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto result = from( t ).shuffle( ).sort( ).where( []( auto v ) { return v > 0; } ).stable_partition( []( auto v ) {
		return v % 2 == 0;
	} );

	std::cout << "Test 002\n";
	std::cout << result << std::endl;

	std::cout << "{";
	for( auto v : result ) {
		std::cout << " " << v;
	}
	std::cout << " }\n";
}

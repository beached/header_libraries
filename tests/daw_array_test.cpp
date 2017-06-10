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

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw_array.h"

using namespace std::literals::string_literals;

BOOST_AUTO_TEST_CASE( make_array_testing ) {
	auto const t = daw::make_array( 1, 2, 3, 4, 5, 6 );
	
	auto pos = std::find( t.begin( ), t.end( ), 4 );
	assert( *pos == 4 );
}

BOOST_AUTO_TEST_CASE( to_array_testing ) {
	int s[] = { 1, 2, 3, 4, 5, 6 }; 

	auto const t = daw::to_array( s );
	auto pos = std::find( t.begin( ), t.end( ), 4 );
	assert( *pos == 4 );
}


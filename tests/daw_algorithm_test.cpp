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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "daw_algorithm.h"

BOOST_AUTO_TEST_CASE( daw_transform_many ) {
	std::vector<uint32_t> in1 = {1, 3, 5, 7, 9};
	std::vector<uint32_t> in2 = {0, 2, 4, 6, 8};
	std::vector<uint32_t> out;

	daw::algorithm::transform_many( in1.begin( ), in1.end( ), in2.begin( ), std::back_inserter( out ),
	                                []( auto const &v1, auto const &v2 ) { return v1 + v2; } );
	BOOST_REQUIRE_MESSAGE( out.size( ) == in1.size( ), "1. Incorrect size on output" );
}

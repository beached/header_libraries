// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#include "daw_range.h"
#include "daw_range_operators.h"

BOOST_AUTO_TEST_CASE( daw_range_test01 ) {
	std::vector<int32_t> t = { -400, 4, -1, 1000, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	using namespace daw::range;
	auto rng = make_range( t );
//	auto result = rng.where( []( auto v ) { return 0 == v%2; } )
//		.sort( )
//		.unique( )
//		.transform( []( auto const & v ) {
//			return v * 10;
//		} );

	using namespace ::daw::range;
	auto result = rng 
					<< where( []( auto const & v ) { return 0 == v%2; } )
					<< where( []( auto const & v ) { return 0 > v; } );
	
	std::cout << result << std::endl;
}


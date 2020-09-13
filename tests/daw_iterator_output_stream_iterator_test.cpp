// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_output_stream_iterator.h"

#include <algorithm>
#include <array>
#include <iostream>

void test_output_stream_001( ) {
	auto osi = daw::make_output_stream_iterator( std::cout );
	auto a = std::array{ 0, 1, 2, 3, 4 };
	std::copy( a.begin( ), a.end( ), osi );
}

int main( ) {
	test_output_stream_001( );
}

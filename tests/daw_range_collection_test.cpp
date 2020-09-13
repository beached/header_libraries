// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_range_collection.h"
#include "daw/daw_range_common.h"

#include <vector>

void test_01( ) {
	std::vector<int> a = { 1, 2, 3, 4, 5 };
	using namespace daw::range;
	auto tst = from( a );
	daw::do_not_optimize( tst );
}

int main( ) {
	test_01( );
}

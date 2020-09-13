// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/parallel/daw_semaphore.h"

#include <utility>

void test_01( ) {
	daw::semaphore sem1;
	daw::shared_semaphore sem2{ std::move( sem1 ) };
}

int main( ) {
	test_01( );
}

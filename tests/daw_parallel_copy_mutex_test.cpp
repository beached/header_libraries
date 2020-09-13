// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/parallel/daw_copy_mutex.h"

#include <mutex>
#include <type_traits>

void daw_copiable_mutex_test_01( ) {
	daw::copiable_mutex<std::mutex> m;
	auto n = std::move( m );
	std::lock_guard<daw::copiable_mutex<std::mutex>> lck{ n };
}

int main( ) {
	daw_copiable_mutex_test_01( );
}

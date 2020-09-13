// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <mutex>

#include "daw/parallel/daw_spin_lock.h"

void daw_spin_lock_001( ) {
	auto sp = daw::spin_lock( );
	auto mut = std::lock_guard<daw::spin_lock>( sp );
}

int main( ) {
	daw_spin_lock_001( );
}

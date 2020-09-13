// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_scoped_multilock.h"

#include <mutex>
#include <thread>

void daw_scoped_multilock_001( ) {
	std::mutex m1;
	std::mutex m2;
	bool result1 = false;
	bool result2 = false;
	auto lck = daw::make_scoped_multilock( m1, m2 );
	std::thread th{ [&]( ) {
		result1 = !m1.try_lock( );
		result2 = !m2.try_lock( );
		if( !result1 ) {
			m1.unlock( );
		}
		if( !result2 ) {
			m2.unlock( );
		}
	} };

	th.join( );
	daw::expecting_message( result1, "m1 wasn't locked" );
	daw::expecting_message( result2, "m2 wasn't locked" );
}

int main( ) {
	daw_scoped_multilock_001( );
}

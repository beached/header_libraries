// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <chrono>
#include <cstddef>
#include <thread>
#include <utility>

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_latch.h"

void construction_001( ) {
	auto sem1 = daw::unique_latch( );
	auto sem1b = daw::unique_latch( );
	auto sem2 = daw::shared_latch( std::move( sem1 ) );
}

void barrier_001( ) {
	constexpr size_t const count = 5;
	auto sem = daw::shared_latch( count );
	for( size_t n = 0; n < count; ++n ) {
		using namespace std::chrono_literals;
		auto th = std::thread( [sem]( ) mutable {
			std::this_thread::sleep_for( 2s );
			sem.notify( );
		} );
		th.detach( );
	}
	sem.wait( );
}

void try_wait_001( ) {
	auto const sem = daw::unique_latch( 0 );
	daw::expecting( sem.try_wait( ) );
}

int main( ) {
	construction_001( );
	barrier_001( );
	try_wait_001( );
}

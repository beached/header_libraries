// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_counter.h"

#include <chrono>
#include <cstddef>
#include <thread>
#include <type_traits>

void construction_001( ) {
	daw::unique_counter sem1;
	auto sem1b = daw::unique_counter( );
	auto sem2 = daw::shared_counter( std::move( sem1 ) );
}

void barrier_001( ) {
	constexpr size_t const count = 5;
	auto sem = daw::shared_counter( count );
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
	auto const sem = daw::unique_counter( 0 );
	daw::expecting( sem.try_wait( ) );
}

int main( ) {
	construction_001( );
	barrier_001( );
	try_wait_001( );
}

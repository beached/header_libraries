// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/parallel/daw_locked_value.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>

struct B {
	daw::lockable_value_t<int> a;

	B( )
	  : a{ 0 } {}

	auto get( ) {
		return *a;
	}
};

void test_lockable_value01( ) {
	B b;

	auto const worker = [&b]( ) {
		using namespace std::chrono_literals;
		for( size_t n = 0; n < 3; ++n ) {
			auto v = b.get( );
			std::cout << "Starting: " << std::this_thread::get_id( ) << '\n';
			++( *v );
			std::cout << "current value: " << *v << '\n';
			std::this_thread::sleep_for( 1s );
			std::cout << "Ending: " << std::this_thread::get_id( ) << '\n';
			v.release( );
			std::this_thread::sleep_for( 1s );
		}
	};

	auto t1 = std::thread( worker );
	auto t2 = std::thread( worker );
	auto t3 = std::thread( worker );
	auto t4 = std::thread( worker );

	t1.join( );
	t2.join( );
	t3.join( );
	t4.join( );
}

void const_lock_value_01( ) {
	auto const a = daw::lockable_value_t<int>( 0xDEAD );
	{
		auto b = *a;
		daw::expecting( 0xDEAD, b.get( ) );
	}
}

void const_lock_value_02( ) {
	auto const a = daw::lockable_value_t<int, std::recursive_mutex>( 0xDEAD );
	{
		auto b = *a;
		daw::expecting( 0xDEAD, b.get( ) );
	}
}

int main( ) {
	test_lockable_value01( );
	const_lock_value_01( );
}

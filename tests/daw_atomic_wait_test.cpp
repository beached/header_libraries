// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_atomic_wait.h>
#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

DAW_ATTRIB_NOINLINE void test_atomic_wait_value_until_timeout( ) {
	auto value = std::atomic<int>{ };
	auto result = daw::atomic_wait_value_equal_until(
	  &value, 42, std::chrono::system_clock::now( ) + 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_value_until_no_timeout( ) {
	auto value = std::atomic<int>{ };

	auto th = std::thread( [&value] {
		std::this_thread::sleep_for( 500ms );
		value = 42;
		std::atomic_notify_one( &value );
	} );
	auto const result = daw::atomic_wait_value_equal_until(
	  &value, 42, std::chrono::system_clock::now( ) + 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 42 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_value_for_timeout( ) {
	auto value = std::atomic<int>{ };
	auto result = daw::atomic_wait_value_equal_for( &value, 42, 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_value_for_no_timeout( ) {
	auto value = std::atomic<int>{ };

	auto th = std::thread( [&value] {
		std::this_thread::sleep_for( 500ms );
		value = 42;
		std::atomic_notify_one( &value );
	} );
	auto const result = daw::atomic_wait_value_equal_for( &value, 42, 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 42 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_value_already_set( ) {
	auto value = std::atomic<int>{ 42 };
	daw::atomic_wait_value_equal( &value, 42 );
	daw_ensure( value == 42 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_value( ) {
	auto value = std::atomic<int>{ 4 };
	auto const fn = [&] {
		std::this_thread::sleep_for( 50ms );
		--value;
		std::atomic_notify_one( &value );
	};
	auto ths = std::array<std::thread, 4>{ std::thread( fn ), std::thread( fn ),
	                                       std::thread( fn ), std::thread( fn ) };
	daw::atomic_wait_value_equal( &value, 0 );
	for( auto &th : ths ) {
		th.join( );
	}
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_if_until_timeout( ) {
	auto value = std::atomic<int>{ };
	static constexpr auto pred = []( auto const &v ) {
		return v == 42;
	};
	auto result = daw::atomic_wait_if_until(
	  &value, pred, std::chrono::system_clock::now( ) + 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_if_until_no_timeout( ) {
	auto value = std::atomic<int>{ };
	auto th = std::thread( [&value] {
		std::this_thread::sleep_for( 500ms );
		value = 42;
		std::atomic_notify_one( &value );
	} );
	static constexpr auto pred = []( auto const &v ) {
		return v == 42;
	};
	auto const result = daw::atomic_wait_if_until(
	  &value, pred, std::chrono::system_clock::now( ) + 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 42 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_if_for_timeout( ) {
	auto value = std::atomic<int>{ };
	static constexpr auto pred = []( auto const &v ) {
		return v == 42;
	};
	auto result = daw::atomic_wait_if_for( &value, pred, 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_if_for_no_timeout( ) {
	auto value = std::atomic<int>{ };
	auto th = std::thread( [&value] {
		std::this_thread::sleep_for( 500ms );
		value = 42;
		std::atomic_notify_one( &value );
	} );
	static constexpr auto pred = []( auto const &v ) {
		return v == 42;
	};
	auto const result = daw::atomic_wait_if_for( &value, pred, 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 42 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_if( ) {
	auto value = std::atomic<int>{ 4 };
	auto const fn = [&] {
		std::this_thread::sleep_for( 50ms );
		--value;
		std::atomic_notify_one( &value );
	};
	auto ths = std::array<std::thread, 4>{ std::thread( fn ), std::thread( fn ),
	                                       std::thread( fn ), std::thread( fn ) };
	static constexpr auto pred = []( auto const &v ) {
		return v == 0;
	};
	daw::atomic_wait_if( &value, pred );
	for( auto &th : ths ) {
		th.join( );
	}
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_until_timeout( ) {
	auto value = std::atomic<int>{ };
	auto result = daw::atomic_wait_until(
	  &value, 0, std::chrono::system_clock::now( ) + 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_until( ) {
	auto value = std::atomic<int>{ };
	auto th = std::thread( [&] {
		std::this_thread::sleep_for( 100ms );
		++value;
		std::atomic_notify_one( &value );
	} );
	auto result = daw::atomic_wait_until(
	  &value, 0, std::chrono::system_clock::now( ) + 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 1 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_for_timeout( ) {
	auto value = std::atomic<int>{ };
	auto result = daw::atomic_wait_for( &value, 0, 30ms );
	daw_ensure( result == daw::wait_status::timeout );
	daw_ensure( value == 0 );
}

DAW_ATTRIB_NOINLINE void test_atomic_wait_for( ) {
	auto value = std::atomic<int>{ };
	auto th = std::thread( [&] {
		std::this_thread::sleep_for( 100ms );
		++value;
		std::atomic_notify_one( &value );
	} );
	auto result = daw::atomic_wait_for( &value, 0, 1min );
	th.join( );
	daw_ensure( result == daw::wait_status::found );
	daw_ensure( value == 1 );
}

int main( ) {
	test_atomic_wait_value_until_timeout( );
	test_atomic_wait_value_until_no_timeout( );
	test_atomic_wait_value_for_timeout( );
	test_atomic_wait_value_for_no_timeout( );
	test_atomic_wait_value_already_set( );
	test_atomic_wait_value( );
	test_atomic_wait_if_until_timeout( );
	test_atomic_wait_if_until_no_timeout( );
	test_atomic_wait_if_for_timeout( );
	test_atomic_wait_if_for_no_timeout( );
	test_atomic_wait_if( );
	test_atomic_wait_until_timeout( );
	test_atomic_wait_until( );
	test_atomic_wait_for_timeout( );
	test_atomic_wait_for( );
}

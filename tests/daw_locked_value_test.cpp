// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "boost_test.h"
#include <chrono>
#include <iostream>
#include <thread>

#include "daw_locked_value.h"

struct A {
	std::mutex m_mutex;
	int counter = 0;
	std::string str;

	auto get( ) {
		return daw::make_locked_value( m_mutex, counter );
	}

	auto get_str( ) {
		return daw::make_locked_value( m_mutex, str );
	}
};

BOOST_AUTO_TEST_CASE( test_locked_value01 ) {
	A a;

	auto const worker = [&a]( ) {
		using namespace std::chrono_literals;
		for( size_t n = 0; n < 3; ++n ) {
			auto v = a.get( );
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

	std::cout << "operator-> tst" << a.get_str( )->empty( ) << '\n';
}

struct B {
	daw::lockable_value_t<int> a;

	B( )
	  : a{0} {}

	auto get( ) {
		return *a;
	}
};

BOOST_AUTO_TEST_CASE( test_lockable_value01 ) {
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

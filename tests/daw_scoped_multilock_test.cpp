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

#include "daw_scoped_multilock.h"

BOOST_AUTO_TEST_CASE( daw_scoped_multilock_001 ) { 
	std::mutex m1;
	std::mutex m2;
	bool result1 = false;
	bool result2 = false;
	auto lck = daw::make_scoped_multilock( m1, m2 );
	std::thread th{[&]( ) {
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
	BOOST_REQUIRE_MESSAGE( result1, "m1 wasn't locked" );
	BOOST_REQUIRE_MESSAGE( result2, "m2 wasn't locked" );
}


// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "daw_memory.h"

#if defined(__clang__) 
struct test3 {
	constexpr static daw::memory<uint32_t> const reg2{ 0x1000u };
};
#endif


BOOST_AUTO_TEST_CASE( daw_memory_001 ) {
#if defined(__clang__) 
	constexpr daw::memory<uint8_t> test1( 0x0000000000000400u );
	constexpr daw::memory<uint8_t> test2( 0x0000000000000800u );
	constexpr static daw::memory<uint8_t> const reg1( 0x1000u );
#endif

	daw::memory<uint8_t const> test3( 0x0000000000000400u );
	daw::memory<uint8_t> test4( 0x0000000000000800u );

	BOOST_REQUIRE( test3 < test4 );


#if false
	// This will probably fail on most OS's
	for( size_t n = 0; n < 0x400; ++n ) {
		std::cout << (int)test1[n] << " ";
	}
#endif
 
}


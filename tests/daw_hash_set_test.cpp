// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_hash_set.h"

void test_001( ) {
	size_t count = 1024ULL;
	daw::hash_set_t<size_t> adapt( count );
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
}

bool test_003( ) {
	size_t count = 1024ULL;
	daw::hash_set_t<size_t> adapt( count );
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		if( !adapt.exists( n ) ) {
			return false;
		}
	}
	return true;
}

int main( ) {
	test_001( );
	daw::expecting( test_003( ) );
}

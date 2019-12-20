// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include "daw/daw_hash_table2.h"

#include <cstdint>
#include <iostream>
#include <new>
#include <string>

using namespace std::literals::string_literals;
void daw_hash_table_testing( ) {
	daw::hash_table<int> testing1;
	struct big {
		uint32_t a;
		uint32_t b;
		uint16_t c;
	};
	daw::hash_table<big> testing2;

	testing1["hello"] = 5;
	testing1[454] = 6;
	testing1.shrink_to_fit( );
	testing2["hello"] = {5, 6, 7};
	testing2.shrink_to_fit( );
	auto a = sizeof( big );
	auto b = sizeof( big * );
	std::cout << a << b << testing1["hello"] << " " << testing1[454] << " "
	          << testing2["hello"].b << std::endl;
}

int main( ) {
	daw_hash_table_testing( );
}

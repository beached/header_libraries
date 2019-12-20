// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include "daw/daw_metro_hash.h"
#include "daw/daw_string_view.h"
#include "daw/daw_view.h"

#include <cstring>
#include <iostream>

inline constexpr daw::string_view test_value =
  "012345678901234567890123456789012345678901234567890123456789012";

inline constexpr auto h0 =
  daw::metro::hash64( {test_value.begin( ), test_value.end( )}, 0 );
inline constexpr auto h1 =
  daw::metro::hash64( {test_value.begin( ), test_value.end( )}, 1 );

// const uint8_t MetroHash64::test_seed_0[8] = {0x6B, 0x75, 0x3D, 0xAE,
//                                             0x06, 0x70, 0x4B, 0xAD};
// const uint8_t MetroHash64::test_seed_1[8] = {0x3B, 0x0D, 0x48, 0x1C,
//                                             0xF4, 0xB9, 0xB8, 0xDF};

// static_assert( h0 == 0x658F'044F'5C73'0E40ULL );
// static_assert( h0 == 0x073CAAB960623211 );

int main( int argc, char **argv ) {
	(void)h0;
	(void)h1;

	std::cout << std::hex << h0 << '\n';
	std::cout << std::hex << h1 << '\n';
	auto const h = daw::metro::hash64(
	  daw::view<char const *>( argv[0], argv[0] + strlen( argv[0] ) ), 0 );
	std::cout << std::hex << h << '\n';
}

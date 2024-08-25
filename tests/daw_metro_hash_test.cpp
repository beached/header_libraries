// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_metro_hash.h"

#include "daw/daw_string_view.h"
#include "daw/daw_view.h"

#include <cstring>
#include <iostream>

inline constexpr daw::string_view test_value =
  "012345678901234567890123456789012345678901234567890123456789012";

inline constexpr auto h0 =
  daw::metro::hash64( { test_value.begin( ), test_value.end( ) }, 0 );
inline constexpr auto h1 =
  daw::metro::hash64( { test_value.begin( ), test_value.end( ) }, 1 );

// const uint8_t MetroHash64::test_seed_0[8] = {0x6B, 0x75, 0x3D, 0xAE,
//                                             0x06, 0x70, 0x4B, 0xAD};
// const uint8_t MetroHash64::test_seed_1[8] = {0x3B, 0x0D, 0x48, 0x1C,
//                                             0xF4, 0xB9, 0xB8, 0xDF};

// static_assert( h0 == 0x658F'044F'5C73'0E40ULL );
// static_assert( h0 == 0x073CAAB960623211 );

int main( int, char **argv ) {
	(void)h0;
	(void)h1;

	std::cout << std::hex << h0 << '\n';
	std::cout << std::hex << h1 << '\n';
	auto const h = daw::metro::hash64(
	  daw::view<char const *>( argv[0], argv[0] + strlen( argv[0] ) ), 0 );
	std::cout << std::hex << h << '\n';
}

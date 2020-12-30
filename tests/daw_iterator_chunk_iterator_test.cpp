// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/iterator/daw_chunk_iterator.h>

#include <array>
#include <iostream>

int main( ) {
	std::array<char, 4 * 2 * 2> buff;
	auto chunk = daw::chunk_iterator<char *>( buff.data( ), 4 );
	constexpr auto last = daw::chunk_iterator<char *>( );
	char r = 0;
	char g = 1;
	char b = 2;
	char a = 3;
	while( chunk != last ) {
		chunk[0] = r;
		chunk[1] = g;
		chunk[2] = b;
		chunk[3] = a;
		++chunk;
	}
	for( char c : buff ) {
		std::cout << static_cast<int>( c );
	}
	std::cout << '\n';
}

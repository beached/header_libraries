// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_size_literals.h"

namespace daw_size_literals_001 {
	using namespace daw::size_literals;
	static_assert( 1_KB == 1024ull );
	static_assert( 1_MB == 1024ull * 1024u );
	static_assert( 1_GB == 1024ull * 1024u * 1024u );
	static_assert( 1_TB == 1024ull * 1024u * 1024u * 1024u );
	static_assert( 1_PB == 1024ull * 1024u * 1024u * 1024u * 1024u );
} // namespace daw_size_literals_001

int main( ) {}

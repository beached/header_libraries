// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>

#include <cstddef>

DAW_ATTRIB_RET_NONNULL
DAW_ATTRIB_NONNULL( )
char const *test_attrib_nonnull( char const *ptr, std::ptrdiff_t offset ) {
	return ptr + offset;
}

DAW_ATTRIB_RET_NONNULL
DAW_ATTRIB_NONNULL( ( 1 ) )
char const *test_attrib_nonnull2( char const *ptr, std::ptrdiff_t offset ) {
	return ptr + offset;
}

int main( int, char **argv ) {
	auto a = test_attrib_nonnull( argv[0], 0 );
	daw_ensure( a == argv[0] );
	a = test_attrib_nonnull2( argv[0], 0 );
	daw_ensure( a == argv[0] );
}

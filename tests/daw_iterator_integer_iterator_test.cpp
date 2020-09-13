// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_integer_iterator.h"

constexpr bool integer_range_01( ) {
	int sum = 0;
	for( auto s : daw::integer_range<int>( 5, 10 ) ) {
		sum += s;
	}
	return 35 == sum;
}
#if not defined( _MSC_VER ) or defined( __clang__ )
static_assert( integer_range_01( ) );
#endif

int main( ) {
#if defined( _MSC_VER ) and not defined( __clang__ )
	if( integer_range_01( ) ) {
		return 0;
	}
	return 1;
#endif
}

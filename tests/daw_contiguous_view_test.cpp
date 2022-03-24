// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_contiguous_view.h>

#include <numeric>

int foo( daw::contiguous_view<int> vals ) {
	return std::accumulate( vals.begin( ), vals.end( ), 0 );
}

int main( ) {
	int vals[] = {1,2,3};
	return foo( vals );
}
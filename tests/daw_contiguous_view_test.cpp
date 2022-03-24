// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_contiguous_view.h>

#include <numeric>

DAW_CONSTEVAL int foo( daw::contiguous_view<int const> vals ) {
	return std::accumulate( vals.begin( ), vals.end( ), 0 );
}

int main( ) {
	constexpr int vals[] = { 1, 2, 3 };
	return foo( vals ) == 6 ? 0 : foo( vals );
}
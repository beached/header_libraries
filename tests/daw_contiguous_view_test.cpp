// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_contiguous_view.h"

#include <daw/daw_ensure.h>

#include <numeric>

DAW_CONSTEVAL int foo( daw::contiguous_view<int const> vals ) {
	return std::accumulate( vals.begin( ), vals.end( ), 0 );
}

int main( ) {
	static constexpr int vals[] = { 1, 2, 3 };
	constexpr auto cv = daw::contiguous_view( vals );
	static_assert( foo( cv ) == 6 );
	constexpr auto vals2 = std::array{ 1, 2, 3 };
	static_assert( daw::contiguous_view( vals2 )[0] == 1 );
}
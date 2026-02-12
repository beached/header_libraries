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

int foo( daw::contiguous_view<int const> vals ) {
	return std::accumulate( vals.begin( ), vals.end( ), 0 );
}

int foo( daw::contiguous_view<int> vals ) {
	return std::accumulate( vals.begin( ), vals.end( ), 0 );
}

int main( ) {
	static constexpr int vals[] = { 1, 2, 3 };
	constexpr auto cv = daw::contiguous_view( vals );
	daw_ensure( foo( cv ) == 6 );
	constexpr auto vals2 = std::array{ 1, 2, 3 };
	static_assert( daw::contiguous_view( vals2 )[0] == 1 );
	static_assert( daw::contiguous_view( vals2, 1 )[0] == 1 );

	int vals3[] = { 1, 2, 3 };
	auto cv3 = daw::contiguous_view( vals3 );
	daw_ensure( foo( cv3 ) == 6 );
	constexpr auto vals4 = std::array{ 1, 2, 3 };
	daw_ensure( daw::contiguous_view( vals4 )[0] == 1 );
	daw_ensure( daw::contiguous_view( vals4, 1 )[0] == 1 );
}
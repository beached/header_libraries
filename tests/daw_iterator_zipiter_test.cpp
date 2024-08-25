// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_zipiter.h"

#include "daw/daw_bounded_array.h"

#include <algorithm>
#include <cstdint>
#include <tuple>

constexpr bool test( ) {
	daw::bounded_array_t<uint8_t, 10> a{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	daw::bounded_array_t<uint8_t, 10> b{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	auto zi = daw::zip_iterator( a.begin( ), b.begin( ) );
	++zi;
	using it_t = std::iterator_traits<daw::remove_cvref_t<decltype( zi )>>;
	using ref = typename it_t::reference;
	static_assert( std::is_same_v<ref, ref> );
	using ptr = typename it_t::pointer;
	static_assert( std::is_same_v<ptr, ptr> );
	// return *daw::get<0>( zi ) == 8 and std::get<1>( *zi ) == 2;
	auto const b0 = std::apply(
	                  []( auto const &lhs, auto const &rhs ) {
		                  return lhs + rhs;
	                  },
	                  *zi ) == 10;
	std::advance( zi, 2 );
	auto const b1 = std::apply(
	                  []( auto const &lhs, auto const &rhs ) {
		                  return lhs + rhs;
	                  },
	                  *zi ) == 10;
	return b0 and b1;
}
static_assert( test( ) );

int main( ) {}

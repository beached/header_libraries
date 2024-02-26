// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_nth_pack_element.h>
#include <daw/traits/daw_traits_nth_element.h>

#include <type_traits>

int main( ) {
	daw_ensure( daw::nth_pack_element<1>( 1, 2, 3 ) == 2 );
	daw_ensure( daw::forward_nth_pack_element<2>( 1, 2, 3 ) == 3 );
	static_assert(
	  std::is_same_v<int, daw::traits::nth_element<0, int, int &, int const &,
	                                               int &&, int const &&>> );
	static_assert(
	  std::is_same_v<int &, daw::traits::nth_element<1, int, int &, int const &,
	                                                 int &&, int const &&>> );
	static_assert(
	  std::is_same_v<int const &,
	                 daw::traits::nth_element<2, int, int &, int const &, int &&,
	                                          int const &&>> );
	static_assert(
	  std::is_same_v<int &&, daw::traits::nth_element<3, int, int &, int const &,
	                                                  int &&, int const &&>> );
	static_assert(
	  std::is_same_v<int const &&,
	                 daw::traits::nth_element<4, int, int &, int const &, int &&,
	                                          int const &&>> );
}

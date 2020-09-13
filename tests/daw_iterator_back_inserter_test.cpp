// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_traits.h"
#include "daw/iterator/daw_back_inserter.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>
#include <vector>

static_assert( daw::traits::is_output_iterator_test<
               daw::remove_cvref_t<decltype(
                 daw::back_inserter( std::declval<std::vector<int> &>( ) ) )>,
               int>( ) );

void vector_test_001( ) {
	std::vector<int> v{ };
	std::array<int, 5> a = { 1, 2, 3, 4, 5 };
	std::copy( begin( a ), end( a ), daw::back_inserter( v ) );
	daw::expecting( std::equal( begin( a ), end( a ), begin( v ), end( v ) ) );
}

int main( ) {
	vector_test_001( );
}

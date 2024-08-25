// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_inserter.h"

#include "daw/daw_benchmark.h"
#include "daw/traits/daw_traits_concepts.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

static_assert( daw::traits::is_output_iterator_test<
               daw::remove_cvref_t<decltype( daw::inserter(
                 std::declval<std::vector<int> &>( ) ) )>,
               int>( ) );

void vector_test_001( ) {
	std::unordered_map<int, int> v{ };
	std::array<int, 5> a = { 1, 2, 3, 4, 5 };
	std::transform(
	  begin( a ), end( a ), daw::inserter( v ), []( auto const &val ) {
		  return std::pair<int const, int>( val, val );
	  } );
	daw::expecting( a.size( ), v.size( ) );
}

int main( ) {
	vector_test_001( );
}

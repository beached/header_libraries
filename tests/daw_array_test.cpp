// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_algorithm.h"
#include "daw/daw_array.h"
#include "daw/daw_benchmark.h"

#include <array>
#include <string>

using namespace std::literals::string_literals;

constexpr bool make_array_testing( ) {
	auto const t = daw::make_array( 1, 2, 3, 4, 5, 6 );

	auto pos = daw::algorithm::find( t.begin( ), t.end( ), 4 );
	daw::expecting( 4, *pos );
	return true;
}
static_assert( make_array_testing( ) );

constexpr bool to_array_testing( ) {
	int s[] = { 1, 2, 3, 4, 5, 6 };

	auto const t = daw::to_array( s );
	auto pos = daw::algorithm::find( t.begin( ), t.end( ), 4 );
	daw::expecting( 4, *pos );
	return true;
}
static_assert( to_array_testing( ) );

void make_string_array_testing( ) {
	auto const t = daw::make_string_array( "1", "2", "3", "4", "5", "6" );

	auto pos = daw::algorithm::find( t.begin( ), t.end( ), "4" );
	daw::expecting( 1U, pos->size( ) );
	daw::expecting( "4", *pos );
}

static_assert( std::is_same_v<std::array<std::array<std::array<int, 5>, 6>, 7>, daw::md_stdarray_t<int, 7, 6, 5>> );

int main( ) {
	make_string_array_testing( );
}

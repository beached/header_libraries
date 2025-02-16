// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/algorithms/daw_array_compare.h>

#include <cassert>

inline constexpr int ints1[]{ 1, 2, 3, 4 };
inline constexpr int ints2[]{ 1, 2, 3, 4 };
static_assert( daw::array_cmp( ints1, ints2 ) );

inline constexpr int ints3[2][4]{ { 1, 2, 3, 4 }, { 1, 2, 3, 4 } };
inline constexpr int ints4[2][4]{ { 1, 2, 3, 4 }, { 1, 2, 3, 4 } };
static_assert( daw::array_cmp( ints3, ints4 ) );

inline constexpr int ints5[2][4]{ { 1, 2, 3, 4 }, { 1, 2, 3, 4 } };
inline constexpr int ints6[2][4]{ { 2, 2, 3, 4 }, { 1, 2, 3, 4 } };
static_assert( not daw::array_cmp( ints5, ints6 ) );

#if defined( DAW_HAS_CPP20_3WAY )
static_assert( daw::array_cmp( ints3, ints4, std::compare_three_way{ } ) ==
               std::strong_ordering::equal );
static_assert( daw::array_cmp( ints5, ints6, std::compare_three_way{ } ) !=
               std::strong_ordering::equal );
#endif

int main( ) {
	//
}

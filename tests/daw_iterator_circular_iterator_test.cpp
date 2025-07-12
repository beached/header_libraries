// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_circular_iterator.h"

#include "daw/daw_benchmark.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

void daw_circular_iterator_001( ) {
	std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n = 0; n < 14; ++n ) {
		std::cout << *first++ << ' ';
	}
	std::cout << '\n';
}

void daw_circular_iterator_002( ) {
	std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };

	auto first = daw::make_circular_iterator( numbers );
	for( size_t n = 0; n < 14; ++n ) {
		std::cout << *first-- << ' ';
	}
	std::cout << '\n';
}

void daw_circular_iterator_003( ) {
	std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };

	auto first = daw::make_circular_iterator( numbers );
	for( int n = 0; n < 14; ++n ) {
		first += n * n;
		std::cout << *first << ' ';
	}
	std::cout << '\n';
}

void daw_circular_iterator_004( ) {
	auto a = std::vector<int>{};
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_assignable =
	  std::is_move_assignable<decltype( it )>::value;
	daw::expecting( is_move_assignable );
}

void daw_circular_iterator_005( ) {
	auto a = std::vector<int>{};
	auto it = daw::make_circular_iterator( a );
	constexpr bool is_move_constructible =
	  std::is_move_constructible<decltype( it )>::value;
	daw::expecting( is_move_constructible );
}

void daw_circular_iterator_006( ) {
	auto a = std::vector<int>{};
	auto it = daw::make_circular_iterator( a );
	static_assert( std::is_nothrow_move_assignable_v<decltype( it )> );
}

void daw_circular_iterator_007( ) {
	auto a = std::vector<int>{};
	auto it = daw::make_circular_iterator( a );
	static_assert( std::is_nothrow_move_constructible_v<decltype( it )> );
}

void daw_circular_iterator_008( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 4;
	constexpr bool tst1 = ::std::is_same_v<decltype( *it ), decltype( a[0] )>;
	constexpr bool tst2 = ::std::is_same_v<decltype( it ), decltype( it2 )>;
	daw::expecting( tst1 );
	daw::expecting( tst2 );
}

void daw_circular_iterator_009( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 4;
	constexpr bool tst1 = ::std::is_same_v<decltype( *it ), decltype( a[0] )>;
	constexpr bool tst2 = ::std::is_same_v<decltype( it ), decltype( it2 )>;
	daw::expecting( tst1 );
	daw::expecting( tst2 );
}

void daw_circular_iterator_010( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	using std::swap;
	swap( *it, *( it + 1 ) );
	daw::expecting( a[0], 2 );
	daw::expecting( a[1], 1 );
}

void daw_circular_iterator_011( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 3;
	daw::expecting( std::distance( it, it2 ), 3 );
}

void daw_circular_iterator_012( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	auto it2 = it + 3;
	std::iter_swap( it, it2 );
	daw::expecting( a[0], 4 );
	daw::expecting( a[3], 1 );
}

void daw_circular_iterator_013( ) {
	int a[] = { 1, 2, 3, 4 };
	auto it = daw::make_circular_iterator( a );
	auto const last = it.end( );
	std::reverse( it, last );
	daw::expecting( a[0], 4 );
	daw::expecting( a[1], 3 );
	daw::expecting( a[2], 2 );
	daw::expecting( a[3], 1 );
}

int main( ) {
	daw_circular_iterator_001( );
	daw_circular_iterator_002( );
	daw_circular_iterator_003( );
	daw_circular_iterator_004( );
	daw_circular_iterator_005( );
	daw_circular_iterator_006( );
	daw_circular_iterator_007( );
	daw_circular_iterator_008( );
	daw_circular_iterator_009( );
	daw_circular_iterator_010( );
	daw_circular_iterator_011( );
	daw_circular_iterator_012( );
	daw_circular_iterator_013( );
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/sized_iterator.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace tests {
	using daw::pipelines::sized_iterator;
	using daw::pipelines::sized_iterator_end;

	using vec_it = std::vector<int>::iterator;
	using vec_cit = std::vector<int>::const_iterator;

	// The end of a random access sized_iterator is a sized sentinel, so the
	// distance to it is known without walking
	static_assert( std::sized_sentinel_for<sized_iterator_end<vec_it>,
	                                       sized_iterator<vec_it>> );
	static_assert(
	  std::sized_sentinel_for<sized_iterator_end<vec_cit>,
	                          sized_iterator<vec_it, vec_cit>> );

	DAW_ATTRIB_NOINLINE void test_sized_iterator_end_minus_iterator( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto first = sized_iterator<vec_it>( v.begin( ), 5 );
		auto const last = sized_iterator_end<vec_it>{ };
		daw_ensure( last - first == 5 );
		daw_ensure( first - last == -5 );
		++first;
		daw_ensure( last - first == 4 );
		daw_ensure( first - last == -4 );
		first += 4;
		daw_ensure( last - first == 0 );
		daw_ensure( first == last );
	}

	DAW_ATTRIB_NOINLINE void
	test_sized_iterator_end_minus_iterator_with_different_last( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto first = sized_iterator<vec_it, vec_cit>( v.begin( ), 3 );
		auto const first_end = sized_iterator_end<vec_it>{ };
		auto const last_end = sized_iterator_end<vec_cit>{ };
		daw_ensure( first_end - first == 3 );
		daw_ensure( last_end - first == 3 );
		daw_ensure( first - first_end == -3 );
		daw_ensure( first - last_end == -3 );
		++first;
		daw_ensure( last_end - first == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_sized_iterator_distance_is_count( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto const first = sized_iterator<vec_it>( v.begin( ), 6 );
		auto const last = sized_iterator_end<vec_it>{ };
		daw_ensure( std::ranges::distance( first, last ) == 6 );
	}

	// m_count is how many elements are left, so a later position has a smaller
	// count and must compare greater
	DAW_ATTRIB_NOINLINE void test_sized_iterator_ordering( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto const first = sized_iterator<vec_it>( v.begin( ), 5 );
		auto const third = first + 2;
		auto const last = sized_iterator_end<vec_it>{ };
		daw_ensure( first < third );
		daw_ensure( third > first );
		daw_ensure( not( third < first ) );
		daw_ensure( first <= first and first >= first );
		daw_ensure( first < last );
		daw_ensure( third < last );
		daw_ensure( not( first + 5 < last ) );
		daw_ensure( first + 5 >= last );
	}

	DAW_ATTRIB_NOINLINE void test_sized_iterator_equality( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto const first = sized_iterator<vec_it>( v.begin( ), 5 );
		daw_ensure( first == first );
		daw_ensure( first + 2 == first + 2 );
		daw_ensure( first != first + 1 );
		daw_ensure( first + 5 == sized_iterator_end<vec_it>{ } );
	}

	// Algorithms that order iterators work through a Take view
	DAW_ATTRIB_NOINLINE void test_take_with_ordering_algorithms( ) {
		auto v = std::vector<int>{ 5, 3, 1, 4, 2, 9, 8 };
		auto r = daw::pipelines::pipeline( v, daw::pipelines::Take( 5 ) );
		static_assert( std::random_access_iterator<decltype( std::begin( r ) )> );
		auto const first = std::begin( r );
		auto const last = first + 5;
		std::sort( first, last );
		daw_ensure( ( v == std::vector<int>{ 1, 2, 3, 4, 5, 9, 8 } ) );
		auto const found = std::lower_bound( first, last, 4 );
		daw_ensure( found - first == 3 );
		daw_ensure( *found == 4 );
	}
} // namespace tests

int main( ) {
	tests::test_sized_iterator_end_minus_iterator( );
	tests::test_sized_iterator_end_minus_iterator_with_different_last( );
	tests::test_sized_iterator_distance_is_count( );
	tests::test_sized_iterator_ordering( );
	tests::test_sized_iterator_equality( );
	tests::test_take_with_ordering_algorithms( );
}

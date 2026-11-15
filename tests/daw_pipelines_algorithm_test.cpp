// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/algorithm.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/pipelines/pipeline.h>

#include <functional>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	inline constexpr auto abs_value = []( int x ) {
		return x < 0 ? -x : x;
	};

	DAW_ATTRIB_NOINLINE void test_max_min_default( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		daw_ensure( *pipeline( v, Max ) == 3 );
		daw_ensure( *pipeline( v, Min ) == -5 );
	}

	// Max( compare ) must use the given comparator, not std::less<>
	DAW_ATTRIB_NOINLINE void test_max_compare( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		daw_ensure( *pipeline( v, Max( std::greater<>{ } ) ) == -5 );
	}

	DAW_ATTRIB_NOINLINE void test_min_compare( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		daw_ensure( *pipeline( v, Min( std::greater<>{ } ) ) == 3 );
	}

	// The projection is applied before comparing; the element itself is
	// returned, not the projected value
	DAW_ATTRIB_NOINLINE void test_max_projection( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		daw_ensure( *pipeline( v, Max( std::less<>{ }, abs_value ) ) == -5 );
	}

	DAW_ATTRIB_NOINLINE void test_min_projection( ) {
		auto v = std::vector<int>{ -1, -5, 3 };
		daw_ensure( *pipeline( v, Min( std::less<>{ }, abs_value ) ) == -1 );
	}

	// MinMax returns a tuple of iterators: { min, max }
	DAW_ATTRIB_NOINLINE void test_minmax_default( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		auto const [lo, hi] = pipeline( v, MinMax );
		daw_ensure( *lo == -5 );
		daw_ensure( *hi == 3 );
	}

	DAW_ATTRIB_NOINLINE void test_minmax_projection( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		auto const [lo, hi] = pipeline( v, MinMax( std::less<>{ }, abs_value ) );
		daw_ensure( *lo == 1 );
		daw_ensure( *hi == -5 );
	}

	DAW_ATTRIB_NOINLINE void test_sort_compare( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		(void)pipeline( v, Sort( std::greater<>{ } ) );
		daw_ensure( ( v == std::vector<int>{ 3, 1, -5 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_sort_projection( ) {
		auto v = std::vector<int>{ 1, -5, 3 };
		(void)pipeline( v, Sort( std::less<>{ }, abs_value ) );
		daw_ensure( ( v == std::vector<int>{ 1, 3, -5 } ) );
	}
} // namespace tests

int main( ) {
	tests::test_max_min_default( );
	tests::test_max_compare( );
	tests::test_min_compare( );
	tests::test_max_projection( );
	tests::test_min_projection( );
	tests::test_minmax_default( );
	tests::test_minmax_projection( );
	tests::test_sort_compare( );
	tests::test_sort_projection( );
}

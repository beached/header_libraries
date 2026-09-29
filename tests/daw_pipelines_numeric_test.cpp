// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/numeric.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <ranges>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	inline constexpr auto even = []( int x ) {
		return x % 2 == 0;
	};

	// std::ranges::filter_view only has a non-const begin( ), so it must not be
	// taken by const &
	static_assert( not daw::ConstRange<decltype( std::declval<std::vector<int> &>( ) |
	                                             std::views::filter( even ) )> );

	DAW_ATTRIB_NOINLINE void test_count_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto fv = v | std::views::filter( even );
		daw_ensure( pipeline( fv, Count ) == 3 );
		daw_ensure( pipeline( v | std::views::filter( even ), Count ) == 3 );
	}

	DAW_ATTRIB_NOINLINE void test_count_if_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto fv = v | std::views::filter( even );
		daw_ensure( pipeline( fv, CountIf( []( int x ) {
			                      return x > 2;
		                      } ) ) == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_sum_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto fv = v | std::views::filter( even );
		daw_ensure( pipeline( fv, Sum ) == 12 );
	}
} // namespace tests

int main( ) {
	tests::test_count_of_filter_view( );
	tests::test_count_if_of_filter_view( );
	tests::test_sum_of_filter_view( );
}

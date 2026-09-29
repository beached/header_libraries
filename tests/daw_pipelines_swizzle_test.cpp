// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/swizzle.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <iterator>
#include <list>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	// A range whose reference is a real pair &, not a tuple value
	DAW_ATTRIB_NOINLINE void test_swizzle_range_of_pairs( ) {
		auto v = std::vector<std::pair<int, int>>{ { 1, 10 }, { 2, 20 } };
		auto r = pipeline( v, Swizzle<1, 0> );
		auto it = std::begin( r );
		daw_ensure( std::get<0>( *it ) == 10 );
		daw_ensure( std::get<1>( *it ) == 1 );
		++it;
		daw_ensure( std::get<0>( *it ) == 20 );
		daw_ensure( std::get<1>( *it ) == 2 );
		++it;
		daw_ensure( it == std::end( r ) );
		daw_ensure( r.size( ) == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_swizzle_list_of_pairs_is_sized( ) {
		auto l = std::list<std::pair<int, int>>{ { 1, 10 }, { 2, 20 }, { 3, 30 } };
		auto r = pipeline( l, Swizzle<1> );
		static_assert( std::ranges::sized_range<decltype( r )> );
		daw_ensure( r.size( ) == 3 );
		daw_ensure( std::get<0>( *std::begin( r ) ) == 10 );
	}

	// Swizzling a zip reorders the zipped ranges
	DAW_ATTRIB_NOINLINE void test_swizzle_zip( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::vector<int>{ 10, 20, 30 };
		auto z = zip_view( a, b );
		auto r = pipeline( z, Swizzle<1, 0> );
		auto it = std::begin( r );
		daw_ensure( std::get<0>( *it ) == 10 );
		daw_ensure( std::get<1>( *it ) == 1 );
		daw_ensure( r.size( ) == 3 );

		auto repeated = pipeline( zip_view( a, b ), Swizzle<1, 1, 0> );
		auto rit = std::begin( repeated );
		daw_ensure( std::get<0>( *rit ) == 10 );
		daw_ensure( std::get<1>( *rit ) == 10 );
		daw_ensure( std::get<2>( *rit ) == 1 );
	}

	// Reordering ranges of different types changes the type of the zip
	DAW_ATTRIB_NOINLINE void test_swizzle_zip_of_different_ranges( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::list<double>{ 0.5, 1.5 };
		auto z = zip_view( a, b );
		auto r = pipeline( z, Swizzle<1, 0> );
		static_assert(
		  std::is_same_v<decltype( r ),
		                 zip_view<std::list<double> &, std::vector<int> &>> );
		auto it = std::begin( r );
		daw_ensure( std::get<0>( *it ) == 0.5 );
		daw_ensure( std::get<1>( *it ) == 1 );
		daw_ensure( r.size( ) == 2 );
	}

	// Swizzling a zip of lvalues still refers to the original ranges
	DAW_ATTRIB_NOINLINE void test_swizzle_zip_refers_to_ranges( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::vector<int>{ 10, 20, 30 };
		auto z = zip_view( a, b );
		auto r = pipeline( z, Swizzle<1> );
		std::get<0>( *std::begin( r ) ) = 99;
		daw_ensure( b[0] == 99 );
	}

	DAW_ATTRIB_NOINLINE void test_swizzle_const_zip( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::vector<int>{ 10, 20, 30 };
		auto const z = zip_view( a, b );
		auto r = pipeline( z, Swizzle<1, 0> );
		static_assert(
		  std::is_same_v<decltype( r ), zip_view<std::vector<int> const &,
		                                         std::vector<int> const &>> );
		daw_ensure( std::get<0>( *std::begin( r ) ) == 10 );
	}

	// A zip that owns its ranges moves them out once, and copies a range whose
	// index is repeated so no copy is of a moved from range
	DAW_ATTRIB_NOINLINE void test_swizzle_owning_zip_with_repeated_index( ) {
		auto r = pipeline( zip_view( std::vector<int>{ 1, 2, 3 },
		                             std::vector<int>{ 10, 20, 30 } ),
		                   Swizzle<1, 1, 0> );
		static_assert(
		  std::is_same_v<decltype( r ), zip_view<std::vector<int>, std::vector<int>,
		                                         std::vector<int>>> );
		daw_ensure( r.size( ) == 3 );
		auto it = std::begin( r );
		++it;
		daw_ensure( std::get<0>( *it ) == 20 );
		daw_ensure( std::get<1>( *it ) == 20 );
		daw_ensure( std::get<2>( *it ) == 2 );
	}
} // namespace tests

int main( ) {
	tests::test_swizzle_range_of_pairs( );
	tests::test_swizzle_list_of_pairs_is_sized( );
	tests::test_swizzle_zip( );
	tests::test_swizzle_zip_of_different_ranges( );
	tests::test_swizzle_zip_refers_to_ranges( );
	tests::test_swizzle_const_zip( );
	tests::test_swizzle_owning_zip_with_repeated_index( );
}

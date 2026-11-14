// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/filter.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <iterator>
#include <list>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	inline constexpr auto even = []( int x ) {
		return x % 2 == 0;
	};

	template<typename R>
	std::vector<int> collect( R &&r ) {
		auto result = std::vector<int>{ };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			result.push_back( *it );
		}
		return result;
	}

	template<typename T, typename R>
	std::vector<T> collect_as( R &&r ) {
		auto result = std::vector<T>{ };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			result.push_back( *it );
		}
		return result;
	}

	DAW_ATTRIB_NOINLINE void test_filter_values( ) {
		auto v = std::vector<int>{ 1, 3, 5, 6, 7, 8, 10 };
		auto r = pipeline( v, Filter( even ) );
		daw_ensure( ( collect( r ) == std::vector<int>{ 6, 8, 10 } ) );
		// Walking it again gives the same result
		daw_ensure( ( collect( r ) == std::vector<int>{ 6, 8, 10 } ) );
	}

	// Repeated calls to begin( ) give the same position, and the const begin( )
	// agrees with it
	DAW_ATTRIB_NOINLINE void test_filter_begin_is_stable( ) {
		auto l = std::list<int>{ 1, 3, 5, 6, 7, 8 };
		auto r = pipeline( l, Filter( even ) );
		auto const &cr = r;
		daw_ensure( *std::begin( r ) == 6 );
		daw_ensure( std::begin( r ) == std::begin( r ) );
		daw_ensure( &*std::begin( r ) == &*std::begin( cr ) );
	}

	// A copy of a view that owns its range refers to its own range, not the
	// original's, so it still works after the original is gone
	DAW_ATTRIB_NOINLINE void test_filter_copy_of_owning_view( ) {
		auto copy = std::optional<decltype( pipeline(
		  std::vector<int>{ }, Filter( even ) ) )>{ };
		{
			auto original =
			  pipeline( std::vector<int>{ 1, 3, 6, 7, 8 }, Filter( even ) );
			daw_ensure( *std::begin( original ) == 6 );
			copy.emplace( original );
			daw_ensure( &*std::begin( *copy ) != &*std::begin( original ) );
		}
		daw_ensure( ( collect( *copy ) == std::vector<int>{ 6, 8 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_filter_move_of_owning_view( ) {
		auto original = pipeline( std::vector<int>{ 1, 3, 6, 7, 8 }, Filter( even ) );
		daw_ensure( *std::begin( original ) == 6 );
		auto moved = std::move( original );
		daw_ensure( ( collect( moved ) == std::vector<int>{ 6, 8 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_filter_no_match( ) {
		auto v = std::vector<int>{ 1, 3, 5 };
		auto r = pipeline( v, Filter( even ) );
		daw_ensure( std::begin( r ) == std::end( r ) );
		daw_ensure( std::begin( r ) == std::end( r ) );
	}

	DAW_ATTRIB_NOINLINE void test_filter_apply_values( ) {
		using value_t = std::tuple<int, int>;
		auto values = std::vector<value_t>{
		  { 1, 2 }, { 2, 2 }, { 3, 4 }, { 5, 5 }
		};
		auto r = pipeline( values, FilterApply( []( int lhs, int rhs ) {
			                   return lhs == rhs;
		                   } ) );
		daw_ensure(
		  ( collect_as<value_t>( r ) ==
		    std::vector<value_t>{ { 2, 2 }, { 5, 5 } } ) );
	}

	DAW_ATTRIB_NOINLINE void
	test_filter_apply_const_reference_and_const_traversal( ) {
		using value_t = std::tuple<int, int>;
		auto values = std::vector<value_t>{
		  { 1, 2 }, { 2, 2 }, { 3, 4 }, { 5, 5 }
		};
		auto r = pipeline(
		  values, FilterApply( []( int const &lhs, int const &rhs ) {
			  return lhs == rhs;
		  } ) );
		auto const &cr = r;
		auto const expected = std::vector<value_t>{ { 2, 2 }, { 5, 5 } };
		daw_ensure( collect_as<value_t>( cr ) == expected );
		daw_ensure( collect_as<value_t>( cr ) == expected );
	}

	struct mutable_equal {
		std::size_t calls = 0;

		bool operator( )( int lhs, int rhs ) {
			++calls;
			return lhs == rhs;
		}
	};

	DAW_ATTRIB_NOINLINE void test_filter_apply_mutable_predicate( ) {
		using value_t = std::tuple<int, int>;
		auto values = std::vector<value_t>{ { 1, 2 }, { 2, 2 }, { 3, 4 } };
		auto r = pipeline( values, FilterApply( mutable_equal{ } ) );
		daw_ensure( ( collect_as<value_t>( r ) ==
		              std::vector<value_t>{ { 2, 2 } } ) );
	}

	DAW_ATTRIB_NOINLINE void test_filter_apply_pair( ) {
		using value_t = std::pair<int, int>;
		auto values = std::vector<value_t>{
		  { 1, 2 }, { 2, 2 }, { 3, 4 }, { 5, 5 }
		};
		auto r = pipeline( values, FilterApply( []( int lhs, int rhs ) {
			                   return lhs == rhs;
		                   } ) );
		daw_ensure(
		  ( collect_as<value_t>( r ) ==
		    std::vector<value_t>{ { 2, 2 }, { 5, 5 } } ) );
	}
} // namespace tests

int main( ) {
	tests::test_filter_values( );
	tests::test_filter_begin_is_stable( );
	tests::test_filter_copy_of_owning_view( );
	tests::test_filter_move_of_owning_view( );
	tests::test_filter_no_match( );
	tests::test_filter_apply_values( );
	tests::test_filter_apply_const_reference_and_const_traversal( );
	tests::test_filter_apply_mutable_predicate( );
	tests::test_filter_apply_pair( );
}

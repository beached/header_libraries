// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/map.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <array>
#include <concepts>
#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <memory>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	/// True when R has a size( ) member.  The check needs a template context to
	/// be false rather than an error
	template<typename R>
	inline constexpr bool has_size_member =
	  requires( daw::remove_cvref_t<R> const &r ) { r.size( ); };

	inline constexpr auto twice = []( int x ) {
		return x * 2;
	};

	/// size( ) and std::ranges::size match the number of elements found by
	/// walking the range
	template<typename R>
	void ensure_size_matches( R &&r, std::size_t expected ) {
		static_assert( std::ranges::sized_range<R> );
		daw_ensure( r.size( ) == expected );
		daw_ensure( std::ranges::size( r ) == expected );
		auto walked = std::size_t{ 0 };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			++walked;
		}
		daw_ensure( walked == expected );
	}

	template<typename Container>
	void check_map_size( ) {
		for( int n = 0; n < 10; ++n ) {
			auto c = Container( );
			for( int i = 0; i < n; ++i ) {
				c.push_back( i );
			}
			ensure_size_matches( pipeline( c, Map( twice ) ),
			                     static_cast<std::size_t>( n ) );
		}
	}

	DAW_ATTRIB_NOINLINE void test_map_size_over_random_range( ) {
		check_map_size<std::vector<int>>( );
	}

	// std::list knows its size but cannot subtract iterators
	DAW_ATTRIB_NOINLINE void test_map_size_over_sized_bidirectional_range( ) {
		check_map_size<std::list<int>>( );
	}

	// Only a range that knows its size without walking gives Map a size( )
	DAW_ATTRIB_NOINLINE void test_map_has_no_size_over_unsized_range( ) {
		auto fl = std::forward_list<int>{ 1, 2, 3 };
		auto r = pipeline( fl, Map( twice ) );
		static_assert( not has_size_member<decltype( r )> );
	}

	/// A view over a range that is only iterable when not const is not
	/// iterable when const either
	template<typename R>
	inline constexpr bool has_const_begin =
	  requires( daw::remove_cvref_t<R> const &r ) { r.begin( ); };

	// std::ranges::filter_view only has a non-const begin( ), as begin( )
	// caches the first match
	DAW_ATTRIB_NOINLINE void test_map_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto fv = v | std::views::filter( []( int x ) {
			          return x % 2 == 0;
		          } );
		auto r = pipeline( fv, Map( twice ) );
		static_assert( not has_const_begin<decltype( r )> );
		daw_ensure( ( pipeline( r, To<std::vector> ) ==
		              std::vector<int>{ 4, 8, 12 } ) );

		auto owned = pipeline( v | std::views::filter( []( int x ) {
			                       return x % 2 == 0;
		                       } ),
		                       Map( twice ) );
		daw_ensure( ( pipeline( owned, To<std::vector> ) ==
		              std::vector<int>{ 4, 8, 12 } ) );
	}

	// A range that is iterable when const still gives a view with a const
	// begin( )
	DAW_ATTRIB_NOINLINE void test_map_keeps_const_begin( ) {
		auto v = std::vector<int>{ 1, 2, 3 };
		auto const r = pipeline( v, Map( twice ) );
		static_assert( has_const_begin<decltype( r )> );
		daw_ensure( *std::begin( r ) == 2 );
	}

	struct tuple_record {
		int number;
		std::string text;

		friend bool operator==( tuple_record const &,
		                        tuple_record const & ) = default;
	};
	static_assert(
	  std::invocable<pimpl::FromTupleFn<tuple_record>, int, std::string> );
	static_assert(
	  not std::invocable<pimpl::FromTupleFn<tuple_record>, std::string> );

	DAW_ATTRIB_NOINLINE void test_from_tuple_constructs_values( ) {
		auto values = std::vector<std::tuple<int, std::string>>{
		  { 1, "one" }, { 2, "two" }, { 3, "three" } };
		auto const result =
		  pipeline( values, FromTuple<tuple_record>, To<std::vector> );

		daw_ensure( ( result == std::vector<tuple_record>{
		                          { 1, "one" }, { 2, "two" }, { 3, "three" } } ) );
	}

	struct reference_record {
		int &value;
	};

	DAW_ATTRIB_NOINLINE void test_from_tuple_preserves_references( ) {
		auto value = 17;
		auto values = std::array{ std::tie( value ) };
		auto result = pipeline( values, FromTuple<reference_record> );

		( *result.begin( ) ).value = 42;
		daw_ensure( value == 42 );
	}

	DAW_ATTRIB_NOINLINE void test_from_tuple_as_reusable_stage( ) {
		constexpr auto make_records =
		  pipeline( FromTuple<tuple_record>, To<std::vector> );
		auto const result =
		  make_records( std::array{ std::tuple{ 4, std::string{ "four" } },
		                            std::tuple{ 5, std::string{ "five" } } } );

		daw_ensure(
		  ( result == std::vector<tuple_record>{ { 4, "four" }, { 5, "five" } } ) );
	}

	struct owning_record {
		std::unique_ptr<int> value;
	};

	DAW_ATTRIB_NOINLINE void test_from_tuple_forwards_temporary_elements( ) {
		auto tuples = std::array{ 6, 7 } | std::views::transform( []( int value ) {
			              return std::tuple{ std::make_unique<int>( value ) };
		              } );
		auto result = pipeline( tuples, FromTuple<owning_record>, To<std::vector> );

		daw_ensure( result.size( ) == 2 );
		daw_ensure( *result[0].value == 6 );
		daw_ensure( *result[1].value == 7 );
	}
} // namespace tests

int main( ) {
	tests::test_map_size_over_random_range( );
	tests::test_map_size_over_sized_bidirectional_range( );
	tests::test_map_has_no_size_over_unsized_range( );
	tests::test_map_of_filter_view( );
	tests::test_map_keeps_const_begin( );
	tests::test_from_tuple_constructs_values( );
	tests::test_from_tuple_preserves_references( );
	tests::test_from_tuple_as_reusable_stage( );
	tests::test_from_tuple_forwards_temporary_elements( );
}

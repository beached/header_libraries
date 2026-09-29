// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/split.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <iterator>
#include <list>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	template<typename R>
	std::vector<std::string> pieces( R &&r ) {
		auto result = std::vector<std::string>{ };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			auto piece = *it;
			result.emplace_back( std::begin( piece ), std::end( piece ) );
		}
		return result;
	}

	DAW_ATTRIB_NOINLINE void test_split_values( ) {
		auto s = std::string( "ab,cde,,f" );
		auto r = pipeline( s, Split( ',' ) );
		auto const expected = std::vector<std::string>{ "ab", "cde", "", "f" };
		daw_ensure( pieces( r ) == expected );
		// Walking it again gives the same result
		daw_ensure( pieces( r ) == expected );
	}

	// Repeated calls to begin( ) give the same position, and the const begin( )
	// agrees with it
	DAW_ATTRIB_NOINLINE void test_split_begin_is_stable( ) {
		auto l = std::list<char>{ 'a', 'b', ',', 'c' };
		auto r = pipeline( l, Split( ',' ) );
		auto const &cr = r;
		auto const first = *std::begin( r );
		auto const cfirst = *std::begin( cr );
		daw_ensure( &*std::begin( first ) == &*std::begin( cfirst ) );
		daw_ensure( std::begin( r ) == std::begin( r ) );
		daw_ensure( ( pieces( r ) == std::vector<std::string>{ "ab", "c" } ) );
	}

	// A copy of a view that owns its range refers to its own range, not the
	// original's, so it still works after the original is gone
	DAW_ATTRIB_NOINLINE void test_split_copy_of_owning_view( ) {
		auto copy =
		  std::optional<decltype( pipeline( std::string( ), Split( ',' ) ) )>{ };
		{
			auto original = pipeline(
			  std::string( "a long first piece so no small string,b" ), Split( ',' ) );
			(void)std::begin( original );
			copy.emplace( original );
		}
		daw_ensure( ( pieces( *copy ) == std::vector<std::string>{
		                                   "a long first piece so no small string",
		                                   "b" } ) );
	}

	DAW_ATTRIB_NOINLINE void test_split_move_of_owning_view( ) {
		auto original = pipeline( std::string( "x,y" ), Split( ',' ) );
		(void)std::begin( original );
		auto moved = std::move( original );
		daw_ensure( ( pieces( moved ) == std::vector<std::string>{ "x", "y" } ) );
	}
} // namespace tests

int main( ) {
	tests::test_split_values( );
	tests::test_split_begin_is_stable( );
	tests::test_split_copy_of_owning_view( );
	tests::test_split_move_of_owning_view( );
}

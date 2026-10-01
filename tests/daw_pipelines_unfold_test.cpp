// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// unfold.h first so that this test also checks that it stands on its own
#include "daw/pipelines/unfold.h"

#include "daw/daw_ensure.h"
#include "daw/daw_pipelines.h"

#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	inline constexpr auto count_to_five = []( int current )
	  -> std::optional<std::pair<int, int>> {
		if( current == 5 ) {
			return std::nullopt;
		}
		return std::pair{ current, current + 1 };
	};

	constexpr bool test_constexpr_unfold( ) {
		auto range = Unfold( count_to_five )( 0 );
		auto expected = 0;
		for( auto value : range ) {
			if( value != expected++ ) {
				return false;
			}
		}
		return expected == 5;
	}
	static_assert( test_constexpr_unfold( ) );

	using count_range_t = decltype( Unfold( count_to_five )( 0 ) );
	static_assert( std::ranges::input_range<count_range_t> );
	static_assert( not std::ranges::sized_range<count_range_t> );

	void test_empty_range( ) {
		auto range = Unfold( []( int )
		                       -> std::optional<std::pair<int, int>> {
			return std::nullopt;
		} )( 0 );
		daw_ensure( range.begin( ) == range.end( ) );
	}

	void test_lvalue_seed_is_referenced( ) {
		auto seed = 1;
		auto range = Unfold( count_to_five )( seed );
		seed = 2;
		auto const result = pipeline( range, To<std::vector> );
		daw_ensure( ( result == std::vector<int>{ 2, 3, 4 } ) );
	}

	struct bounded_step {
		int limit;

		std::optional<std::pair<int, int>> operator( )( int current ) const {
			if( current == limit ) {
				return std::nullopt;
			}
			return std::pair{ current, current + 1 };
		}
	};

	void test_lvalue_callable_is_referenced( ) {
		auto step = bounded_step{ 4 };
		auto range = Unfold( step )( 0 );
		step.limit = 2;
		auto const result = pipeline( range, To<std::vector> );
		daw_ensure( ( result == std::vector<int>{ 0, 1 } ) );
	}

	auto make_owned_range( ) {
		return Unfold( [limit = 4]( int current )
		                 -> std::optional<std::pair<int, int>> {
			if( current == limit ) {
				return std::nullopt;
			}
			return std::pair{ current, current + 1 };
		} )( 0 );
	}

	void test_temporary_callable_is_owned( ) {
		auto range = make_owned_range( );
		auto const result = pipeline( range, To<std::vector> );
		daw_ensure( ( result == std::vector<int>{ 0, 1, 2, 3 } ) );
	}

	struct move_only_step {
		std::unique_ptr<int> limit;

		move_only_step( int value )
		  : limit( std::make_unique<int>( value ) ) {}

		move_only_step( move_only_step && ) = default;
		move_only_step( move_only_step const & ) = delete;

		std::optional<std::pair<int, int>> operator( )( int current ) const {
			if( current == *limit ) {
				return std::nullopt;
			}
			return std::pair{ current, current + 1 };
		}
	};

	void test_move_only_callable( ) {
		auto range = Unfold( move_only_step{ 3 } )( 0 );
		auto const result = pipeline( range, To<std::vector> );
		daw_ensure( ( result == std::vector<int>{ 0, 1, 2 } ) );
	}

	void test_pipeline_composition( ) {
		auto const result = pipeline( Unfold( count_to_five )( 0 ),
		                              Map( []( int value ) {
			                              return value * value;
		                              } ),
		                              To<std::vector> );
		daw_ensure( ( result == std::vector<int>{ 0, 1, 4, 9, 16 } ) );
	}

	void test_range_seed( ) {
		using namespace std::string_view_literals;

		auto range = Unfold( []( std::string_view remaining )
		                       -> std::optional<
		                         std::pair<char, std::string_view>> {
			if( remaining.empty( ) ) {
				return std::nullopt;
			}
			auto const value = remaining.front( );
			remaining.remove_prefix( 1 );
			return std::pair{ value, remaining };
		} )( "unfold"sv );

		auto result = std::string{ };
		for( auto const value : range ) {
			result.push_back( value );
		}
		daw_ensure( result == "unfold" );
	}

	void test_move_only_state( ) {
		auto range = Unfold( []( std::unique_ptr<int> const &current )
		                       -> std::optional<
		                         std::pair<int, std::unique_ptr<int>>> {
			if( *current == 3 ) {
				return std::nullopt;
			}
			return std::pair{ *current, std::make_unique<int>( *current + 1 ) };
		} )( std::make_unique<int>( 0 ) );

		auto iterator = range.begin( );
		daw_ensure( *iterator == 0 );
		iterator++;
		daw_ensure( *iterator == 1 );
		++iterator;
		daw_ensure( *iterator == 2 );
		++iterator;
		daw_ensure( iterator == range.end( ) );
	}
} // namespace tests

int main( ) {
	tests::test_empty_range( );
	tests::test_lvalue_seed_is_referenced( );
	tests::test_lvalue_callable_is_referenced( );
	tests::test_temporary_callable_is_owned( );
	tests::test_move_only_callable( );
	tests::test_pipeline_composition( );
	tests::test_range_seed( );
	tests::test_move_only_state( );
}

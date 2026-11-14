// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/enumerate.h"

#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	constexpr bool test_enumerate_multiple_random_ranges( ) {
		auto lhs = std::array{ 10, 20, 30 };
		auto rhs = std::array{ 1, 2 };
		auto extra = std::array{ 100, 200, 300, 400 };
		auto enumerated = Enumerate( lhs, rhs, extra );

		if( enumerated.size( ) != 2 ) {
			return false;
		}
		auto it = enumerated.begin( );
		auto [index0, lhs0, rhs0, extra0] = *it;
		if( index0 != 0 or lhs0 != 10 or rhs0 != 1 or extra0 != 100 ) {
			return false;
		}
		++it;
		auto [index1, lhs1, rhs1, extra1] = *it;
		if( index1 != 1 or lhs1 != 20 or rhs1 != 2 or extra1 != 200 ) {
			return false;
		}
		return ++it == enumerated.end( );
	}

	static_assert( test_enumerate_multiple_random_ranges( ) );

	constexpr bool test_enumerate_from_multiple_ranges( ) {
		auto lhs = std::array{ 10, 20, 30 };
		auto rhs = std::array{ 1, 2 };
		auto enumerated = EnumerateFrom( std::int16_t{ 7 } )( lhs, rhs );
		auto [index, lhs_value, rhs_value] = *enumerated.begin( );
		static_assert(
		  std::same_as<std::remove_cvref_t<decltype( index )>, std::int16_t> );
		return enumerated.size( ) == 2 and index == 7 and lhs_value == 10 and
		       rhs_value == 1;
	}

	static_assert( test_enumerate_from_multiple_ranges( ) );

	constexpr bool test_enumerate_with_multiple_ranges( ) {
		auto lhs = std::array{ 10, 20, 30 };
		auto rhs = std::array{ 1, 2 };
		auto enumerated = EnumerateWith<std::uint16_t>( lhs, rhs );
		auto [index, lhs_value, rhs_value] = *enumerated.begin( );
		static_assert(
		  std::same_as<std::remove_cvref_t<decltype( index )>, std::uint16_t> );
		return enumerated.size( ) == 2 and index == 0 and lhs_value == 10 and
		       rhs_value == 1;
	}

	static_assert( test_enumerate_with_multiple_ranges( ) );

	constexpr bool test_enumerate_with_empty_range( ) {
		auto nonempty = std::array{ 1, 2, 3 };
		auto empty = std::array<int, 0>{ };
		auto enumerated = Enumerate( nonempty, empty );
		return enumerated.size( ) == 0 and enumerated.begin( ) == enumerated.end( );
	}

	static_assert( test_enumerate_with_empty_range( ) );

	void test_enumerate_mixed_range_categories( ) {
		auto lhs = std::forward_list{ 10, 20, 30 };
		auto rhs = std::vector{ 1, 2 };
		auto enumerated = Enumerate( lhs, rhs );
		auto it = enumerated.begin( );

		auto [index0, lhs0, rhs0] = *it;
		daw_ensure( index0 == 0 and lhs0 == 10 and rhs0 == 1 );
		++it;
		auto [index1, lhs1, rhs1] = *it;
		daw_ensure( index1 == 1 and lhs1 == 20 and rhs1 == 2 );
		daw_ensure( ++it == enumerated.end( ) );
	}

	void test_enumerate_owns_temporary_ranges( ) {
		auto enumerated =
		  Enumerate( std::vector{ 10, 20, 30 }, std::vector{ 1, 2 } );
		daw_ensure( enumerated.size( ) == 2 );
		auto [index, lhs, rhs] = *enumerated.begin( );
		daw_ensure( index == 0 and lhs == 10 and rhs == 1 );
	}

	void test_enumerate_as_first_pipeline_stage( ) {
		auto operation =
		  pipeline( Enumerate, MapApply( []( std::size_t index, int lhs, int rhs ) {
			            return static_cast<int>( index ) + lhs + rhs;
		            } ) );
		auto lhs = std::array{ 10, 20, 30 };
		auto rhs = std::array{ 1, 2 };
		auto result = operation( lhs, rhs );
		auto it = result.begin( );
		daw_ensure( *it == 11 );
		daw_ensure( *++it == 23 );
		daw_ensure( ++it == result.end( ) );
	}
} // namespace tests

int main( ) {
	tests::test_enumerate_mixed_range_categories( );
	tests::test_enumerate_owns_temporary_ranges( );
	tests::test_enumerate_as_first_pipeline_stage( );
}

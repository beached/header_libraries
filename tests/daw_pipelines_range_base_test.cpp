// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/range_base.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	/// A forward range over [0, N) that counts how many times its iterators are
	/// incremented, so a test can tell if it was walked
	template<bool HasSize>
	struct counting_range {
		std::size_t *increments;
		int last;

		struct iterator {
			using iterator_category = std::forward_iterator_tag;
			using value_type = int;
			using difference_type = std::ptrdiff_t;
			using reference = int;
			using pointer = void;

			std::size_t *increments = nullptr;
			int value = 0;

			constexpr int operator*( ) const {
				return value;
			}

			constexpr iterator &operator++( ) {
				++*increments;
				++value;
				return *this;
			}

			constexpr iterator operator++( int ) {
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr bool operator==( iterator const &rhs ) const {
				return value == rhs.value;
			}
		};

		constexpr iterator begin( ) const {
			return iterator{ increments, 0 };
		}

		constexpr iterator end( ) const {
			return iterator{ increments, last };
		}

		constexpr std::size_t size( ) const
		requires( HasSize )
		{
			return static_cast<std::size_t>( last );
		}
	};

	DAW_ATTRIB_NOINLINE void test_ranges_distance_uses_size_member( ) {
		auto increments = std::size_t{ 0 };
		auto const r = counting_range<true>{ &increments, 5 };
		daw_ensure( pimpl::ranges_distance( r ) == 5 );
		daw_ensure( increments == 0 );
	}

	DAW_ATTRIB_NOINLINE void test_ranges_distance_walks_without_size( ) {
		auto increments = std::size_t{ 0 };
		auto const r = counting_range<false>{ &increments, 5 };
		daw_ensure( pimpl::ranges_distance( r ) == 5 );
		daw_ensure( increments == 5 );
	}

	DAW_ATTRIB_NOINLINE void test_ranges_distance_result_type( ) {
		auto const l = std::list<int>{ 1, 2, 3 };
		auto const sz = pimpl::ranges_distance<std::size_t>( l );
		static_assert( std::is_same_v<decltype( sz ), std::size_t const> );
		daw_ensure( sz == 3 );
	}

	DAW_ATTRIB_NOINLINE void test_ranges_distance_of_containers( ) {
		daw_ensure( pimpl::ranges_distance( std::vector<int>{ 1, 2, 3 } ) == 3 );
		daw_ensure( pimpl::ranges_distance( std::list<int>{ 1, 2, 3, 4 } ) == 4 );
		daw_ensure(
		  pimpl::ranges_distance( std::map<int, int>{ { 1, 2 }, { 3, 4 } } ) == 2 );
		// No size( ) member, so it is walked
		daw_ensure( pimpl::ranges_distance( std::forward_list<int>{ 1, 2 } ) == 2 );
		daw_ensure( pimpl::ranges_distance( std::vector<int>{ } ) == 0 );
	}

	// Enumerate sizes its index range with ranges_distance
	DAW_ATTRIB_NOINLINE void test_enumerate_does_not_walk_sized_range( ) {
		auto increments = std::size_t{ 0 };
		auto const r = counting_range<true>{ &increments, 3 };
		auto const e = pipeline( r, Enumerate );
		daw_ensure( increments == 0 );
		auto n = std::size_t{ 0 };
		// zip_view's reference is a pair value, so take it by value
		for( auto const [idx, value] : e ) {
			daw_ensure( idx == n );
			daw_ensure( value == static_cast<int>( n ) );
			++n;
		}
		daw_ensure( n == 3 );
	}

	DAW_ATTRIB_NOINLINE void test_count_of_sized_and_unsized_ranges( ) {
		auto increments = std::size_t{ 0 };
		daw_ensure( Count( counting_range<true>{ &increments, 4 } ) == 4 );
		daw_ensure( increments == 0 );
		daw_ensure( Count( counting_range<false>{ &increments, 4 } ) == 4 );
		daw_ensure( increments == 4 );
		daw_ensure( Count( std::list<int>{ 1, 2, 3 } ) == 3 );
	}
} // namespace tests

int main( ) {
	tests::test_ranges_distance_uses_size_member( );
	tests::test_ranges_distance_walks_without_size( );
	tests::test_ranges_distance_result_type( );
	tests::test_ranges_distance_of_containers( );
	tests::test_enumerate_does_not_walk_sized_range( );
	tests::test_count_of_sized_and_unsized_ranges( );
}

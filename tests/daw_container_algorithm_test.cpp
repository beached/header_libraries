// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_container_algorithm.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

void container_algorithm_accumulate( ) {
	std::vector<int> test( 100, 1 );
	auto const sum = daw::container::accumulate( test, 0 );
	daw::expecting( sum >= 0 );
	daw::expecting( test.size( ), static_cast<size_t>( sum ) );

	auto const product = daw::container::accumulate(
	  test, 1, []( auto lhs, auto rhs ) { return lhs * rhs; } );
	daw::expecting( product, 1 );
}

void container_algorithm_transform( ) {
	auto test_vec = std::vector<int>( 100 );
	std::iota( test_vec.begin( ), test_vec.end( ), 1 );

	auto result = std::vector<int>( test_vec.size( ) );

	daw::container::transform( test_vec, result.data( ),
	                           []( int const &val ) { return 2 * val; } );

	auto const sum1 = daw::container::accumulate( test_vec, 0 );
	auto const sum2 = daw::container::accumulate( result, 0 );

	daw::expecting( sum1 * 2, sum2 );
}

void daw_container_algorithm_test_sort( ) {
	std::array<int64_t, 1000> v1{};
	daw::algorithm::iota( std::begin( v1 ), std::end( v1 ), 1 );
	daw::container::sort( v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
	daw::expecting( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
}

void daw_container_algorithm_test_stable_sort( ) {
	std::array<int64_t, 1000> v1{};
	daw::algorithm::iota( std::begin( v1 ), std::end( v1 ), 1 );
	daw::container::stable_sort( v1,
	                             []( auto lhs, auto rhs ) { return lhs < rhs; } );
	daw::expecting( std::is_sorted( v1.cbegin( ), v1.cend( ) ) );
}

constexpr bool daw_container_algorithm_test_max_element( ) {
	std::array<int64_t, 10> v1{};
	daw::algorithm::iota( std::begin( v1 ), std::end( v1 ), 1 );
	auto ans = daw::container::max_element(
	  v1, []( auto lhs, auto rhs ) { return lhs < rhs; } );
	daw::expecting( 10, *ans );
	return true;
}
static_assert( daw_container_algorithm_test_max_element( ) );

constexpr bool daw_container_algorithm_test_copy_001( ) {
	std::array<int, 100> a1{};
	daw::algorithm::iota( std::begin( a1 ), std::end( a1 ), 2 );
	std::array<int, 100> a2{};
	daw::container::copy( a1, a2.begin( ) );
	daw::expecting( daw::algorithm::equal( std::cbegin( a1 ), std::cend( a1 ),
	                                       std::cbegin( a2 ), std::cend( a2 ) ) );
	return true;
}
static_assert( daw_container_algorithm_test_copy_001( ) );

constexpr bool daw_container_algorithm_test_copy_n_001( ) {
	std::array<int, 100> a1{};
	daw::algorithm::iota( std::begin( a1 ), std::end( a1 ), 2 );
	std::array<int, 100> a2{};
	daw::container::copy_n( a1, 100, a2.begin( ) );
	daw::expecting( daw::algorithm::equal( std::cbegin( a1 ), std::cend( a1 ),
	                                       std::cbegin( a2 ), std::cend( a2 ) ) );
	return true;
}
static_assert( daw_container_algorithm_test_copy_n_001( ) );

void daw_for_each_pos_001( ) {
	std::array<int, 5> const blah = {0, 1, 2, 3, 4};
	daw::container::for_each_with_pos( blah, []( auto const &value, size_t pos ) {
		std::cout << pos << ": " << value << '\n';
	} );
}

namespace daw_for_each_with_pos_002_ns {
	struct summer_t {
		int *sum;
		constexpr summer_t( int *ptr ) noexcept
		  : sum{ptr} {}

		template<typename Value>
		constexpr void operator( )( Value const &value, size_t pos ) const
		  noexcept {
			*sum += value * static_cast<Value>( pos );
		}
	};
} // namespace daw_for_each_with_pos_002_ns

void daw_for_each_subset_001( ) {
	std::array<int, 5> const blah = {0, 1, 2, 3, 4};
	daw::container::for_each_subset( blah, 1, 4,
	                                 []( auto &container, size_t pos ) {
		                                 std::cout << container[pos] << '\n';
	                                 } );
}

namespace daw_for_each_subset_002_ns {
	struct summer_t {
		int *sum;
		constexpr summer_t( int *ptr ) noexcept
		  : sum{ptr} {}

		template<typename Container>
		constexpr void operator( )( Container &container, size_t pos ) const
		  noexcept {
			*sum += container[pos];
		}
	};

	template<size_t N>
	constexpr int find_sum( int const ( &ptr )[N] ) noexcept {
		int sum = 0;
		daw::container::for_each_subset( ptr, 1, N - 1, summer_t{&sum} );
		return sum;
	}

	constexpr bool daw_for_each_subset_002( ) {
		int blah[] = {0, 1, 2, 3, 4};
		auto sum = find_sum( blah );
		daw::expecting( 6, sum );
		return true;
	}
	static_assert( daw_for_each_subset_002( ) );
} // namespace daw_for_each_subset_002_ns

void daw_append_001( ) {
	std::vector<int> a = {1, 2, 3};
	std::vector<int> const b = {4, 5, 6};
	std::vector<int> const expect = {1, 2, 3, 4, 5, 6};
	daw::container::append( b, a );
	auto const tst = daw::algorithm::equal( expect.cbegin( ), expect.cend( ),
	                                        a.cbegin( ), a.cend( ) );
	daw::expecting( tst );
}

int main( ) {
	container_algorithm_accumulate( );
	container_algorithm_transform( );
	daw_container_algorithm_test_sort( );
	daw_container_algorithm_test_stable_sort( );
	daw_for_each_pos_001( );
	daw_for_each_subset_001( );
	daw_append_001( );
}

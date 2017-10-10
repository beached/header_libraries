// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "boost_test.h"
#include "daw_benchmark.h"
#include "daw_fixed_lookup.h"

BOOST_AUTO_TEST_CASE( daw_fixed_lookup_001 ) {
	daw::fixed_lookup<int, 100> blah{};
	blah['a'] = 5;
	blah["hello"] = 6;
	BOOST_REQUIRE_EQUAL( blah['a'], 5 );
	BOOST_REQUIRE_EQUAL( blah["hello"], 6 );
	BOOST_REQUIRE( blah['a'] != blah["hello"] );
	BOOST_REQUIRE_EQUAL( blah['a'], blah['a'] );
	std::cout << blah['a'] << " " << blah["hello"] << '\n';
}

constexpr auto get_map( ) {
	daw::fixed_lookup<int, 10> blah{};
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	return blah;
}

// Will not compile... on purpose
template<size_t N>
constexpr auto too_many( ) {
	daw::fixed_lookup<int, N> blah{};
	for( size_t n = 0; n <= N; ++n ) {
		blah[n] = n;
	}
	return blah;
}

BOOST_AUTO_TEST_CASE( daw_fixed_lookup_002 ) {
	constexpr auto values2 = get_map( );
	daw::fixed_lookup<int, 10> blah{};
	blah['a'] = 1;
	blah['2'] = 3;
	blah[4] = 4;
	BOOST_REQUIRE_EQUAL( values2['a'], blah['a'] );
}

BOOST_AUTO_TEST_CASE( daw_make_fixed_lookup_001 ) {
	constexpr auto values = daw::make_fixed_lookup<int>( "hello", 3, 5, 6, "why oh why" );
	BOOST_REQUIRE_EQUAL( values.size( ), values.capacity( ) );
	BOOST_REQUIRE_EQUAL( values.size( ), 5 );
}

BOOST_AUTO_TEST_CASE( daw_fixed_lookup_bench_001 ) {
	using value_t = intmax_t;
	constexpr value_t const SZ = 15'000;
	daw::fixed_lookup<value_t, SZ * 2> lookup{};
	std::unordered_map<value_t, value_t> hash_map{};
	std::vector<value_t> ary{};
	ary.resize( SZ );
	hash_map.reserve( SZ );
	daw::show_benchmark( SZ * sizeof( value_t ), "fixed_lookup(fill)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     lookup[n] = n;
		                     }
	                     },
	                     2, 2, SZ );
	daw::show_benchmark( SZ * sizeof( value_t ), "unordered_map(fill)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     hash_map[n] = n;
		                     }
	                     },
	                     2, 2, SZ );
	daw::show_benchmark( SZ * sizeof( value_t ), "vector(fill)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     ary[static_cast<size_t>( n )] = n;
		                     }
	                     },
	                     2, 2, SZ );

	intmax_t sum1 = 0;
	daw::show_benchmark( SZ * sizeof( value_t ), "fixed_lookup(summation)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     sum1 += lookup[n];
		                     }
	                     },
	                     2, 2, SZ );
	intmax_t sum2 = 0;
	daw::show_benchmark( SZ * sizeof( value_t ), "unordered_map(summation)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     sum2 += hash_map[n];
		                     }
	                     },
	                     2, 2, SZ );
	intmax_t sum3 = 0;
	daw::show_benchmark( SZ * sizeof( value_t ), "vector(summation)",
	                     [&]( ) {
		                     for( value_t n = 0; n < SZ; ++n ) {
			                     sum3 += ary[static_cast<size_t>( n )];
		                     }
	                     },
	                     2, 2, SZ );
	BOOST_REQUIRE_EQUAL( sum1, sum2 );
	BOOST_REQUIRE_EQUAL( sum1, sum3 );
	std::cout << "sum1: " << sum1 << " sum2: " << sum2 << " sum3: " << sum3 << '\n';
}

// Will not compile... on purpose
// BOOST_AUTO_TEST_CASE( daw_fixed_lookup_003 ) {
//	constexpr auto values2 = too_many<10>();
//}

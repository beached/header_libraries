// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/test/unit_test.hpp>
#include <boost/unordered_map.hpp>
#include <cstdint>
#include <iostream>
#include <google/dense_hash_map>
#include <string>

#include "daw_hash_table.h"
#include "daw_benchmark.h"

using namespace std::literals::string_literals;

BOOST_AUTO_TEST_CASE( daw_hash_table_testing ) {
	daw::hash_table<int> testing1;
	struct big {
		uint32_t a;
		uint32_t b;
		uint16_t c;
	};
	daw::hash_table<big> testing2;

	testing1["hello"] = 5;
	testing1[454] = 6;
	testing1.shrink_to_fit( );
	testing2["hello"] = { 5, 6, 7 };
	testing2.shrink_to_fit( );
	auto a = sizeof( big );
	auto b = sizeof( big * );
	std::cout << a << b << testing1["hello"] << " " << testing1[454] << " " << testing2["hello"].b << std::endl;
}

#define TEST_SIZE 1000000

auto integerKeys( size_t count = TEST_SIZE ) {
	std::mt19937_64 rd( 0 );

	std::vector<uint64_t> numbers;
	for( size_t i = 0; i < count; ++i ) {
		numbers.push_back(rd());
	}
	return numbers;
}

auto stringKeys( size_t count = TEST_SIZE ) {
	std::mt19937_64 rd( 0 );

	std::vector<std::string> words;
	for( size_t i = 0; i < count; ++i ) {
		size_t val = rd( );
		std::string item = "key: " + std::to_string( val );
		words.push_back( item );
	}
	return words;
}
template<typename HashSet, typename Keys>
    void do_test( HashSet & set, Keys const & keys) {
		for( auto const & key: keys ) {
			set[key] = key;
		}
	};

template<typename HashSet, typename Keys>
std::tuple<double, double, double> time_once( Keys const & keys) {
	HashSet set;
	auto ins = daw::benchmark( [&set, &keys]( ) {
		do_test<HashSet>( set, keys );
	} );

	auto f = daw::benchmark( [&set, &keys]( ) {
		for( auto const & key: keys ) {
			assert( set[key] == key );
		}
	});

	auto e = daw::benchmark( [&set, &keys]( ) {
		for( auto const & key: keys ) {
			set.erase( key );
		}
	});
	return std::make_tuple( ins, f, e );
}

auto min( std::tuple<double, double, double> a, std::tuple<double, double, double> b ) {
	return std::make_tuple( std::min( std::get<0>( a ), std::get<0>( b ) ), std::min( std::get<1>( a ), std::get<1>( b ) ), std::min( std::get<2>( a ), std::get<2>( b ) ) );
}

std::ostream & operator<<( std::ostream & os, std::tuple<double, double, double> const & v ) {
	os << "{ " << std::get<0>( v ) << ", " << std::get<1>( v ) << ", " << std::get<2>( v ) << " }";
	return os;
}

template<typename HashSet, typename Keys>
auto best_of_many(const Keys& keys) {
	auto best_time = std::make_tuple( std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity( ) );

	for (size_t i = 0; i < 10; ++i) {
		best_time = min(best_time, time_once<HashSet>(keys));
	}
	return best_time;
}

BOOST_AUTO_TEST_CASE( daw_hash_table_testing_correctness ) {
	std::cout << "Testing that all values entered exist afterwards" << std::endl;
	std::cout << "Generating Keys" << std::endl;
	auto keys = integerKeys( );
	std::cout << "Entering keys" << std::endl;
	daw::hash_table<decltype(keys[0])> set;
	for( auto const & key: keys ) {
		set[key] = key;
	}
	std::cout << "Verifying" << std::endl;
	for( auto const & key: keys ) {
		assert( key == set[key] );
	}

	BOOST_REQUIRE( set.begin( ).valid( ) );

	decltype(keys) values;
	
	std::copy( set.begin( ), set.end( ), std::back_inserter( values ) );

	std::sort( keys.begin( ), keys.end( ) );
	std::sort( values.begin( ), values.end( ) );

	BOOST_REQUIRE( keys == values );

	std::cout << "Done" << std::endl;

	
}

void do_testing( size_t count ) {
	{
		std::cout << "Generating Keys" << std::endl;
		auto const keys = integerKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		std::cout << keys.size( ) << " int keys std::unorderd_map " << best_of_many<std::unordered_map<size_t, size_t>>( keys ) << " seconds\n";
		std::cout << keys.size( ) << " int keys hash_table " << best_of_many<daw::hash_table<size_t>>( keys ) << " seconds\n";
	}
	{
		std::cout << "Generating Keys" << std::endl;
		auto const keys = stringKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		std::cout << keys.size( ) << " string keys std::unorderd_map " << best_of_many<std::unordered_map<std::string, std::string>>( keys ) << " seconds\n";
		std::cout << keys.size( ) << " string keys hash_table " << best_of_many<daw::hash_table<std::string>>( keys ) << " seconds\n";
	}

}

BOOST_AUTO_TEST_CASE( daw_hash_table_testing_perf ) {
	do_testing( 1000 );
	do_testing( 10000 );
	do_testing( 100000 );
	do_testing( 1000000 );
	do_testing( 10000000 );

}


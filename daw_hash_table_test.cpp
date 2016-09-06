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
#include <cstdint>
#include <iostream>
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

auto integerKeys( ) {
	std::mt19937_64 rd( 0 );

	std::vector<uint64_t> numbers;
	for( size_t i = 0; i < TEST_SIZE; ++i ) {
		numbers.push_back(rd());
	}
	return numbers;
}

auto stringKeys( ) {
	std::mt19937_64 rd( 0 );

	std::vector<std::string> words;
	for( size_t i = 0; i < TEST_SIZE; ++i ) {
		size_t val = rd( );
		std::string item = "key: " + std::to_string( val );
		words.push_back( item );
	}
	return words;
}
template<typename HashSet, typename Keys>
    void do_test( Keys const & keys) {
		HashSet set;
		for( auto const & key: keys ) {
			set[key] = key;
		}
	};

template<typename HashSet, typename Keys>
double time_once( Keys const & keys) {
	return daw::benchmark( [&keys]( ) {
		do_test<HashSet>( keys );
	} );
}

template<typename HashSet, typename Keys>
double best_of_many(const Keys& keys) {
	double best_time = std::numeric_limits<double>::infinity();
	for (size_t i = 0; i < 10; ++i) {
		best_time = std::min(best_time, time_once<HashSet>(keys));
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
	std::cout << "Done" << std::endl;
}

BOOST_AUTO_TEST_CASE( daw_hash_table_testing_perf ) {
	std::cout << "Generating Keys" << std::endl;
	{
		auto const keys = integerKeys( );
		std::cout << "Starting benchmark" << std::endl;
		std::cout << keys.size( ) << " int keys unorderd_map " << best_of_many<std::unordered_map<size_t, size_t>>( keys ) << " seconds\n";
		std::cout << keys.size( ) << " int keys hash_table " << best_of_many<daw::hash_table<size_t>>( keys ) << " seconds\n";
	}
	{
		auto const keys = stringKeys( );
		std::cout << "Starting benchmark" << std::endl;
		std::cout << keys.size( ) << " string keys unorderd_map " << best_of_many<std::unordered_map<std::string, std::string>>( keys ) << " seconds\n";
		std::cout << keys.size( ) << " string keys hash_table " << best_of_many<daw::hash_table<std::string>>( keys ) << " seconds\n";
	}


}

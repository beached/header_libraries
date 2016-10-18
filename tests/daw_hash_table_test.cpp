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
#include <random>
#include <string>

#include "daw_tuple_helper.h"
#include "daw_hash_table.h"
#include "daw_benchmark.h"

using namespace std::literals::string_literals;
/*
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

auto integerKeys( size_t count = 10000 ) {
	std::mt19937_64 rd( 0 );

	std::vector<uint64_t> numbers;
	for( size_t i = 0; i < count; ++i ) {
		numbers.push_back(rd());
	}
	return numbers;
}

auto stringKeys( size_t count = 10000 ) {
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
auto time_once( Keys const & keys) {
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

template<typename HashSet, typename Keys>
auto best_of_many(const Keys& keys, size_t count = 10 ) {
	auto best_time = std::make_tuple( std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity( ) );

	for (size_t i = 0; i < count; ++i) {
		best_time = daw::tuple::min(best_time, time_once<HashSet>(keys));
	}
	return best_time;
}

BOOST_AUTO_TEST_CASE( daw_hash_table_testing_correctness ) {
	std::cout << "Testing that all values entered exist afterwards" << std::endl;
	std::cout << "Generating Keys" << std::endl;
	auto keys = integerKeys( 10 );
	using value_type = daw::traits::root_type_t <decltype( *std::begin( keys ) )>;
	std::cout << "Entering keys" << std::endl;
	daw::hash_table<value_type> set;
	for( auto const & key: keys ) {
		set[key] = key;
	}
	std::cout << "Verifying" << std::endl;
	for( auto const & key: keys ) {
		assert( key == set[key] );
	}

	BOOST_REQUIRE( set.begin( ).valid( ) );

	std::vector<value_type> values;

	std::copy( set.begin( ), set.end( ), std::back_inserter( values ) );

	std::sort( keys.begin( ), keys.end( ) );
	std::sort( values.begin( ), values.end( ) );

	BOOST_REQUIRE( std::equal( keys.begin( ), keys.end( ), values.begin( ), values.end( ) ) );

	std::cout << "Done" << std::endl;


}


template<typename... Ts>
auto items_per_second( std::tuple<Ts...> const & ts, size_t count ) {
	using namespace daw::tuple::operators;
	return static_cast<double>(count)/ts;
}

template<typename... Ts>
auto sec_to_microsec_each( std::tuple<Ts...> const & ts, size_t count ) {
	using namespace daw::tuple::operators;
	return (ts/static_cast<double>(count))*1000000.0;
}

void do_testing( size_t count ) {
	using namespace daw::tuple::operators;
	using namespace daw::tuple;
	std::cout << "Testing { inserts, queries, removes }\n";
	{
		std::cout << "Generating Keys: " << count << std::endl;
		auto const keys = integerKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		auto b = best_of_many<daw::hash_table<size_t>>( keys );
		auto a = best_of_many<std::unordered_map<size_t, size_t>>( keys );
		auto perf_std = items_per_second( a, count );
		auto perf = items_per_second( b, count );
		std::cout << "size_t keys/values\n";
		std::cout << "-->std::unorderd_map " << perf_std << " ops per seconds, " << sec_to_microsec_each( a, count ) << " µs each\n";
		std::cout << "-->daw::hash_table " << perf << " ops per seconds, " << sec_to_microsec_each( b, count ) << " µs each\n";
		std::cout << "ratio " << ((a * 100.0) / b) << " % perf of std" << std::endl;
	}
	{
		std::cout << "Generating Keys" << std::endl;
		auto const keys = stringKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		auto b = best_of_many<daw::hash_table<std::string>>( keys );
		auto a = best_of_many<std::unordered_map<std::string, std::string>>( keys );
		auto perf_std = items_per_second( a, count );
		auto perf = items_per_second( b, count );
		std::cout << "string keys/values\n";
		std::cout << "-->std::unorderd_map " << perf_std << " ops per seconds, " << sec_to_microsec_each( a, count ) << " µs each\n";
		std::cout << "-->daw::hash_table " << perf << " ops per seconds, " << sec_to_microsec_each( b, count ) << " µs each\n";
		std::cout << "ratio " << ((a * 100.0) / b) << " % perf of std" << std::endl;
	}

}

BOOST_AUTO_TEST_CASE( daw_hash_table_testing_perf ) {
	do_testing( 1000 );
	do_testing( 10000 );
	do_testing( 100000 );
	do_testing( 1000000 );
}

template<typename Keys>
auto time_once_dhs( Keys const & keys, size_t load_factor, double resize_ratio ) {
	using value_type = decltype( *std::begin( keys ) );
	daw::hash_table<value_type> set{ 7, resize_ratio, load_factor };
	auto insert = daw::benchmark( [&set, &keys]( ) {
		do_test( set, keys );
	} );

	auto query = daw::benchmark( [&set, &keys]( ) {
		for( auto const & key: keys ) {
			assert( set[key] == key );
		}
	});

	auto erase = daw::benchmark( [&set, &keys]( ) {
		for( auto const & key: keys ) {
			set.erase( key );
		}
	});
	return std::make_tuple( insert, query, erase );
}

template<typename Keys>
auto best_of_many_dhs(const Keys& keys, size_t count, size_t max_load, double resize_ratio ) {
	auto best_time = std::make_tuple( std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity( ) );

	for (size_t i = 0; i < count; ++i) {
		best_time = daw::tuple::min(best_time, time_once_dhs(keys, max_load, resize_ratio));
	}
	return best_time;
}

auto do_testing_dhs( size_t max_load, double resize_ratio ) {
	using namespace daw::tuple::operators;
	using namespace daw::tuple;
	const size_t count = 1000000;
	std::tuple<double, double, double> result;
	std::cout << "Testing { inserts, queries, removes } with a max_load_factor of " << max_load << "% and a growth ratio of " << resize_ratio << "X\n";
	{
		std::cout << "Generating Keys: " << count << std::endl;
		auto const keys = integerKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		auto a = best_of_many_dhs( keys, 15, max_load, resize_ratio );
		result = a;
		auto perf = items_per_second( a, count );
		std::cout << "size_t keys/values\n";
		std::cout << "-->daw::hash_table " << perf << " ops per seconds, " << sec_to_microsec_each( a, count ) << " µs each\n";
	}
	{
		std::cout << "Generating Keys" << std::endl;
		auto const keys = stringKeys( count );
		std::cout << "Starting benchmark" << std::endl;
		auto a = best_of_many_dhs( keys, 15, max_load, resize_ratio );
		result = min( a, result );
		auto perf = items_per_second( a, count );
		std::cout << "string keys/values\n";
		std::cout << "-->daw::hash_table " << perf << " ops per seconds, " << sec_to_microsec_each( a, count ) << " µs each\n";
	}

	return result;
}
*/

/*
BOOST_AUTO_TEST_CASE( daw_hash_table_size_perf ) {
	std::cout << "\n\nTuning Parameters\n\n";
	using namespace daw::tuple::operators;
	using namespace daw::tuple;
	auto best = std::make_tuple( 10000.0, 10000.0, 1000.0 );
	size_t best_factor = 50;
	double best_ratio = 2.0;
	for( double ratio=1.20; ratio<3.0; ratio += 0.05 ) {
		for( size_t f=50; f<100; f+=5 ) {
			auto r = do_testing_dhs( f, ratio );
			if( std::get<1>( r ) <= std::get<1>( best ) ) {
				best = r;
				best_factor = f;
				best_ratio = ratio;
			}
		}
	}
	std::cout << "\n\nBest query perf was found with a load factor of " << best_factor << ", resize ratio of " << best_ratio << " with results of " << best << '\n';
}
*/


// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "daw/daw_benchmark.h"
#include "daw/iterator/daw_random_iterator.h"

void daw_random_iterator_01( ) {
	int numbers[] = { 1, 2, 3, 4, 5, 6, 7 };

	auto first = daw::make_random_iterator( numbers );
	auto last = daw::make_random_iterator( numbers + 7 );

	std::cout << std::distance( first, last ) << '\n';
	for( auto it = first; it != last; ++it ) {
		std::cout << static_cast<int>( *it ) << '\n';
	}
}

template<typename T>
struct test_t {
	daw::RandomIterator<T> first;
	daw::RandomIterator<T> last;
	test_t( T *const f, T *const l )
	  : first{ f }
	  , last{ l } {}
	test_t( ) = delete;
	~test_t( ) = default;
	test_t( test_t const & ) = default;
	test_t( test_t && ) = default;
	test_t &operator=( test_t const & ) = default;
	test_t &operator=( test_t && ) = default;
	daw::RandomIterator<T> &begin( ) {
		return first;
	}
	daw::RandomIterator<T> const &begin( ) const {
		return first;
	}
	daw::RandomIterator<T> &end( ) {
		return last;
	}
	daw::RandomIterator<T> const &end( ) const {
		return last;
	}
};

void daw_random_iterator_02( ) {
	int numbers[] = { 1, 2, 3, 4, 5, 6, 7 };
	test_t<int> test{ numbers, numbers + 7 };

	for( auto const &i : test ) {
		std::cout << i << '\n';
	}
}

int main( ) {
	daw_random_iterator_01( );
	daw_random_iterator_02( );
}

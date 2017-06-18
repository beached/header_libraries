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

#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw_random_iterator.h"

BOOST_AUTO_TEST_CASE( daw_random_iterator_01 ) {
	int numbers[] = {1, 2, 3, 4, 5, 6, 7};

	auto first = daw::make_random_iterator( numbers );
	auto last = daw::make_random_iterator( numbers + 7 );

	std::cout << std::distance( first, last ) << '\n';
	for( auto it = first; it != last; ++it ) {
		std::cout << (int)*it << '\n';
	}
}

template<typename T>
struct test_t {
	daw::RandomIterator<T> first;
	daw::RandomIterator<T> last;
	test_t( T *const f, T *const l ) : first{f}, last{l} {}
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

BOOST_AUTO_TEST_CASE( daw_random_iterator_02 ) {
	int numbers[] = {1, 2, 3, 4, 5, 6, 7};
	test_t<int> test{numbers, numbers + 7};

	for( auto const &i : test ) {
		std::cout << i << '\n';
	}
}

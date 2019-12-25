// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_bounded_vector.h"

constexpr bool daw_bounded_vector_test_001( ) {
	daw::bounded_vector_t<int, 10> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	int sum = 0;
	for( auto c : a ) {
		sum += c;
	}
	daw::expecting( sum, 36 );
	return true;
}
static_assert( daw_bounded_vector_test_001( ) );

constexpr bool daw_bounded_vector_test_002( ) {
	daw::bounded_vector_t<int, 10> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	daw::expecting( a.back( ), 4 );
	daw::expecting( a.pop_back( ), 4 );
	daw::expecting( a.back( ), 2 );
	daw::expecting( 2U, a.size( ) );
	daw::expecting( 10U, a.capacity( ) );
	daw::expecting( a.front( ), 1 );
	daw::expecting( *a.cbegin( ), 1 );
	daw::expecting( a.pop_back( ), 2 );
	daw::expecting( a.pop_back( ), 1 );
	daw::expecting( 0U, a.size( ) );
	daw::expecting( a.empty( ) );
	return true;
}
static_assert( daw_bounded_vector_test_002( ) );

constexpr bool daw_bounded_vector_test_003( ) {
	daw::bounded_vector_t<int, 6> a{};
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 4 );
	a.push_back( 8 );
	a.push_back( 16 );
	a.emplace_back( 5 );
	daw::expecting( a.full( ) );
	return true;
}
static_assert( daw_bounded_vector_test_003( ) );

namespace daw_bounded_vector_test_004_ns {
	constexpr auto cx_test( ) {
		daw::bounded_vector_t<int, 10> a{};
		a.push_back( 1 );
		a.push_back( 2 );
		a.push_back( 4 );
		a.push_back( 8 );
		a.push_back( 16 );
		a.emplace_back( 5 );
		return a;
	}

	constexpr bool daw_bounded_vector_test_004( ) {
		constexpr auto a = cx_test( );
		int sum = 0;
		for( auto c : a ) {
			sum += c;
		}
		daw::expecting( sum, 36 );
		return true;
	}
	static_assert( daw_bounded_vector_test_004( ) );
} // namespace daw_bounded_vector_test_004_ns

int main( ) {}

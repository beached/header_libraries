// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights home/dwright/github/lib_nodepp/ to use, copy, modify, merge, publish,
// distribute, sublicense, and / or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the following
// conditions:
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

#include "daw/boost_test.h"
#include <iostream>

#include "daw/cpp_17.h"

namespace void_t_tests {
	void test2( int ) {}
	struct X {};

	bool fn2( ... ) {
		return true;
	}

	template<typename T, typename = daw::void_t<decltype( fn( T{} ) )>>
	bool fn2( T ) {
		return false;
	}

	BOOST_AUTO_TEST_CASE( void_t_test_001 ) {
		auto const b1 = fn2( X{} );
		BOOST_REQUIRE( b1 );
	}
} // namespace void_t_tests

void test( ) {}
BOOST_AUTO_TEST_CASE( cpp_17_test_01 ) {
	constexpr auto const is_func = daw::is_function_v<decltype( test )>;
	constexpr auto const isnt_func = daw::is_function_v<decltype( is_func )>;

	BOOST_REQUIRE( is_func );
	BOOST_REQUIRE( !isnt_func );
}

BOOST_AUTO_TEST_CASE( bit_cast_001 ) {
	uint32_t const tst = 0x89AB'CDEF;
	auto const f = daw::bit_cast<float>( tst );
	auto const i = daw::bit_cast<uint32_t>( f );
	BOOST_REQUIRE_EQUAL( tst, i );
}

BOOST_AUTO_TEST_CASE( bit_cast_002 ) {
	uint32_t const tst = 0x89AB'CDEF;
	auto const f = daw::bit_cast<float>( &tst );
	auto const i = daw::bit_cast<uint32_t>( f );
	BOOST_REQUIRE_EQUAL( tst, i );
}

BOOST_AUTO_TEST_CASE( bit_cast_003 ) {
	uint64_t const as_bin = 0b0'01111111111'0000000000000000000000000000000000000000000000000000;	// double 1
	double const as_dbl = 1.0; 
	bool const b1 = daw::bit_cast<uint64_t>( as_dbl ) == as_bin;
	BOOST_REQUIRE( b1 );

	uint64_t const as_bin2 = 0b1'10000000000'0000000000000000000000000000000000000000000000000000;	// double -2
	double const as_dbl2 = -2.0;

	bool const b2 = daw::bit_cast<uint64_t>( as_dbl2 ) == as_bin2;
	BOOST_REQUIRE( b1 );
}

struct tmp_t {};

struct tmp2_t {
	constexpr tmp2_t( tmp_t ) noexcept {}
	constexpr tmp2_t( int ) noexcept( false ) {}
};

BOOST_AUTO_TEST_CASE( is_nothrow_convertible_001 ) {
	static_assert( !daw::is_nothrow_convertible_v<int, tmp_t>,
	               "int should not be convertible to tmp_t" );
	static_assert( daw::is_nothrow_convertible_v<int, double>,
	               "int should not be convertible to tmp_t" );
	static_assert( daw::is_nothrow_convertible_v<tmp_t, tmp2_t>,
	               "tmp_t should be nothrow convertible to tmp2_t" );
	static_assert( !daw::is_nothrow_convertible_v<int, tmp2_t>,
	               "int should not be nothrow convertible to tmp2_t" );
}

template<typename T>
auto dc_func( T &&v ) {
	auto x = std::pair<T, int>( std::forward<T>( v ), 99 );
	return x;
}

BOOST_AUTO_TEST_CASE( decay_copy_001 ) {
	double d = 5.6;
	double const cd = 3.4;

	static_assert(
	  !daw::is_same_v<decltype( dc_func( d ) ), decltype( dc_func( cd ) )>,
	  "Results should be difference, double and double const" );

	static_assert( daw::is_same_v<decltype( daw::decay_copy( d ) ),
	                              decltype( daw::decay_copy( cd ) )>,
	               "Results should be the same, double" );
}


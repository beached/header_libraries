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

#include "daw/daw_benchmark.h"
#include "daw/daw_utility.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace in_range_test {
	static_assert( daw::in_range( 5, 0, 10 ) );
	static_assert( not daw::in_range( -5, 0, 10 ) );
	static_assert( not daw::in_range( 15, 0, 10 ) );
	static_assert( not daw::in_range( 15, -10, 10 ) );
	static_assert( daw::in_range( -10, -10, 10 ) );
	static_assert( not daw::in_range( 10, -10, 10 ) );
} // namespace in_range_test

void daw_utility_append_test( ) {
	::std::vector<int> a{1, 2, 3, 4, 5};
	(void)daw::append( a, 6, 7, 8, 9 );
	daw::expecting( a.size( ) == 9 );
}

void daw_hex_test_001( ) {
	uint32_t const a = 0xFF00FFFF;
	::std::string a_str;
	(void)daw::hex( a, ::std::back_inserter( a_str ) );
	::std::cout << "Output: " << a_str << '\n';
	daw::expecting( a_str, "FF00FFFF" );
}

void daw_hex_test_002( ) {
	::std::vector<uint32_t> vec_a( 10, 0xFF00FFFF );
	::std::string a_str;
	(void)daw::hex( vec_a.cbegin( ), vec_a.cend( ),
	                ::std::back_inserter( a_str ) );
	::std::cout << "Output: " << a_str << '\n';
}

void daw_hex_test_003( ) {
	::std::string a_str;
	(void)daw::hex( "abcdef", ::std::back_inserter( a_str ) );
	::std::cout << "Output: " << a_str << '\n';
}

namespace daw_hex_test_004 {
	constexpr bool to_hex_test_004( ) noexcept {
		uint32_t const a = 0xFF00FFFF;
		char result[9] = {0};
		char const expected[] = "FF00FFFF";
		char *ptr = result;
		(void)daw::hex( a, ptr );
		for( size_t n = 0; n < 8; ++n ) {
			if( result[n] != expected[n] ) {
				return false;
			}
		}
		return true;
	}
	static_assert( to_hex_test_004( ), "" );
} // namespace daw_hex_test_004

void daw_hex_test_005( ) {
	uint32_t const a = 0x789ABCDE;
	::std::string a_str;
	(void)daw::hex( a, ::std::back_inserter( a_str ) );
	::std::cout << "Output: " << a_str << '\n';
	daw::expecting( a_str, "789ABCDE" );
}

namespace daw_pack_index_of_001 {
	static_assert(
	  daw::pack_index_of_v<int, ::std::string, bool, float, int, long> == 3, "" );
}

namespace daw_pack_index_of_002 {
	static_assert(
	  daw::pack_index_of_v<double, ::std::string, bool, float, int, long> == 5,
	  "" );
}

namespace daw_pack_index_of_003 {
	static_assert(
	  daw::pack_index_of_v<bool, ::std::string, bool, float, int, long> == 1,
	  "" );
}

/*
constexpr bool daw_get_pack_value_001( ) {
  constexpr auto const tst = daw::daw_get_pack_value( 2, "a", true, 4 );

  static_assert( ::std::is_same_v<decltype( tst ), bool>, "Expected a bool" );
  return tst;
}
static_assert( daw_get_pack_value_001( ) );
*/

void daw_pack_type_at_001( ) {
	using tst_t = daw::pack_type_at<5, double, float, ::std::string, char, char *,
	                                int, void *>;

	::std::cout
	  << "type of daw::pack_type_at<5, double, float, ::std::string, char, "
	     "char *, int, void *>, should be like int: "
	  << typeid( tst_t ).name( ) << '\n';
	::std::cout << "type id name of int: " << typeid( int ).name( ) << '\n';
	constexpr bool tst = ::std::is_same_v<int, tst_t>;
	daw::expecting( tst );

	constexpr size_t pos = daw::pack_index_of_v<int, double, float, ::std::string,
	                                            char, char *, int, void *>;
	::std::cout
	  << "type of daw::pack_index_of_v<int, double, float, ::std::string, "
	     "char, char *, int, void *> should be 5: "
	  << 5 << '\n';

	daw::expecting( 5U, pos );

	::std::cout << tst << '\n';
}

namespace construct_a_deps {
	static_assert( std::is_constructible_v<::std::vector<int>, size_t, int>,
	               "Vector should be normal constructable" );

	static_assert( not std::is_constructible_v<::std::array<int, 2>, int, int>,
	               "Array should not be normal constructable" );

	static_assert(
	  not std::is_constructible_v<
	    daw::use_aggregate_construction<::std::vector<int>>>,
	  "Type use_aggregate_construction should not be constructible" );

	static_assert(
	  not daw::impl::should_use_aggregate_construction_v<::std::vector<int>>,
	  "array should not be aggregate constructable" );

	static_assert(
	  not daw::impl::should_use_aggregate_construction_v<::std::array<int, 2>>,
	  "array should not be aggregate constructable" );

	static_assert(
	  daw::impl::should_use_aggregate_construction_v<
	    daw::use_aggregate_construction<::std::array<int, 2>>>,
	  "aggregate_construction<array> should be aggregate constructable" );
} // namespace construct_a_deps

void construct_a_001( ) {
	auto tmp = daw::construct_from<::std::vector<int>>(
	  ::std::make_tuple( static_cast<size_t>( 2 ), 5 ) );

	static_assert(
	  ::std::is_same_v<::std::decay_t<decltype( tmp )>, ::std::vector<int>>,
	  "Invalid types" );
	daw::expecting( 2U, tmp.size( ) );
	daw::expecting( 5, tmp[0] );
	daw::expecting( 5, tmp[1] );
}

void construct_a_002( ) {
	auto tmp =
	  daw::construct_from<daw::use_aggregate_construction<::std::vector<int>>>(
	    ::std::make_tuple( 1, 2, 3, 4 ) );
	static_assert(
	  ::std::is_same_v<::std::decay_t<decltype( tmp )>, ::std::vector<int>>,
	  "Invalid types" );
	daw::expecting( 4U, tmp.size( ) );
	daw::expecting( 1, tmp[0] );
	daw::expecting( 2, tmp[1] );
	daw::expecting( 3, tmp[2] );
	daw::expecting( 4, tmp[3] );
}

void construct_a_003( ) {
	auto tmp =
	  daw::construct_from<daw::use_aggregate_construction<::std::array<int, 4>>>(
	    ::std::make_tuple( 1, 2, 3, 4 ) );
	static_assert(
	  ::std::is_same_v<::std::decay_t<decltype( tmp )>, ::std::array<int, 4>>,
	  "Invalid types" );
	daw::expecting( 4U, tmp.size( ) );
	daw::expecting( tmp[0], 1 );
	daw::expecting( tmp[1], 2 );
	daw::expecting( tmp[2], 3 );
	daw::expecting( tmp[3], 4 );
}

void as_char_array_001( ) {
	uint16_t const value = 0x43'21;
	auto const ary = daw::as_char_array( value );

	daw::expecting( 0x43, ary[1] );
	daw::expecting( 0x21, ary[0] );
}

constexpr bool value_is_one_of_001( ) {
	int value = 134;
	return daw::value_is( value ).one_of( 1, 2, 3, 134 );
}
static_assert( value_is_one_of_001( ) );
constexpr bool value_is_none_of_001( ) {
	int value = 134;
	return daw::value_is( value ).none_of( 1, 2, 3, 4 );
}
static_assert( value_is_none_of_001( ) );

// S S
// Bigger -> Smaller
static_assert( daw::narrow_cast<int32_t>( static_cast<int64_t>(
                 ::std::numeric_limits<int32_t>::max( ) ) ) ==
               ::std::numeric_limits<int32_t>::max( ) );
static_assert( daw::narrow_cast<int32_t>( static_cast<int64_t>(
                 ::std::numeric_limits<int32_t>::min( ) ) ) ==
               ::std::numeric_limits<int32_t>::min( ) );
static_assert( daw::narrow_cast<int32_t>( 0LL ) == 0 );
// Smaller -> Bigger
static_assert(
  daw::narrow_cast<int32_t>( ::std::numeric_limits<int8_t>::max( ) ) ==
  ::std::numeric_limits<int8_t>::max( ) );
static_assert(
  daw::narrow_cast<int32_t>( ::std::numeric_limits<int8_t>::min( ) ) ==
  ::std::numeric_limits<int8_t>::min( ) );
static_assert( daw::narrow_cast<int64_t>( 0 ) == 0 );
// Same Size
static_assert( daw::narrow_cast<int>( ::std::numeric_limits<int>::max( ) ) ==
               ::std::numeric_limits<int>::max( ) );

// S U
// Bigger -> Smaller
static_assert( daw::narrow_cast<int16_t>( static_cast<uint32_t>(
                 ::std::numeric_limits<int16_t>::max( ) ) ) ==
               ::std::numeric_limits<int16_t>::max( ) );
static_assert( daw::narrow_cast<int32_t>( static_cast<uint64_t>( 0 ) ) == 0 );
// Smaller -> Bigger
static_assert(
  daw::narrow_cast<int64_t>( ::std::numeric_limits<uint32_t>::max( ) ) ==
  ::std::numeric_limits<uint32_t>::max( ) );
static_assert( daw::narrow_cast<int64_t>( static_cast<uint32_t>( 0 ) ) == 0 );
static_assert( daw::narrow_cast<int>( static_cast<unsigned int>(
                 ::std::numeric_limits<int>::max( ) ) ) ==
               ::std::numeric_limits<int>::max( ) );

// U S
// Bigger -> Smaller
static_assert( daw::narrow_cast<int16_t>( static_cast<uint32_t>(
                 ::std::numeric_limits<int16_t>::max( ) ) ) ==
               ::std::numeric_limits<int16_t>::max( ) );
static_assert( daw::narrow_cast<int16_t>( static_cast<uint32_t>( 0 ) ) == 0 );
// Smaller -> Bigger
static_assert(
  daw::narrow_cast<int64_t>( ::std::numeric_limits<uint32_t>::max( ) ) ==
  ::std::numeric_limits<uint32_t>::max( ) );
static_assert( daw::narrow_cast<int64_t>( static_cast<uint32_t>( 0 ) ) == 0 );

// U U
// Bigger -> Smaller
static_assert( daw::narrow_cast<uint16_t>( static_cast<uint32_t>(
                 ::std::numeric_limits<uint16_t>::max( ) ) ) ==
               ::std::numeric_limits<uint16_t>::max( ) );
static_assert( daw::narrow_cast<uint16_t>( static_cast<uint32_t>( 0 ) ) == 0 );
// Smaller -> Bigger
static_assert(
  daw::narrow_cast<uint64_t>( ::std::numeric_limits<uint32_t>::max( ) ) ==
  ::std::numeric_limits<uint32_t>::max( ) );
static_assert( daw::narrow_cast<uint64_t>( static_cast<uint32_t>( 0 ) ) == 0 );

int main( ) {
	daw_utility_append_test( );
	daw_hex_test_001( );
	daw_hex_test_002( );
	daw_hex_test_003( );
	daw_hex_test_005( );
	daw_pack_type_at_001( );
	construct_a_001( );
	construct_a_002( );
	construct_a_003( );
	as_char_array_001( );
}

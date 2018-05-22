// The MIT License (MIT)
//
// Copyright (c) 2014-2018 Darrell Wright
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

#include <cmath>
#include <sstream>
#include <typeinfo>

#include "boost_test.h"
#include "daw_utility.h"

BOOST_AUTO_TEST_CASE( daw_utility_append_test ) {
	std::vector<int> a{1, 2, 3, 4, 5};
	daw::append( a, 6, 7, 8, 9 );
	BOOST_REQUIRE( a.size( ) == 9 );
}

BOOST_AUTO_TEST_CASE( daw_hex_test_001 ) {
	uint32_t const a = 0xFF00FFFF;
	std::string a_str;
	daw::hex( a, std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
	BOOST_REQUIRE_EQUAL( a_str, "FF00FFFF" );
}

BOOST_AUTO_TEST_CASE( daw_hex_test_002 ) {
	std::vector<uint32_t> vec_a( 10, 0xFF00FFFF );
	std::string a_str;
	daw::hex( vec_a.cbegin( ), vec_a.cend( ), std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
}

BOOST_AUTO_TEST_CASE( daw_hex_test_003 ) {
	std::string a_str;
	daw::hex( "abcdef", std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
}

constexpr bool to_hex_test_004( ) noexcept {
	uint32_t const a = 0xFF00FFFF;
	char result[9] = {0};
	char const expected[] = "FF00FFFF";
	char *ptr = result;
	daw::hex( a, ptr );
	for( size_t n = 0; n < 8; ++n ) {
		if( result[n] != expected[n] ) {
			return false;
		}
	}
	return true;
}

BOOST_AUTO_TEST_CASE( daw_hex_test_004 ) {
	BOOST_REQUIRE( to_hex_test_004( ) );
}

BOOST_AUTO_TEST_CASE( daw_hex_test_005 ) {
	uint32_t const a = 0x789ABCDE;
	std::string a_str;
	daw::hex( a, std::back_inserter( a_str ) );
	std::cout << "Output: " << a_str << '\n';
	BOOST_REQUIRE_EQUAL( a_str, "789ABCDE" );
}

BOOST_AUTO_TEST_CASE( daw_pack_index_of_001 ) {
	constexpr auto const pack_pos =
	  daw::pack_index_of_v<int, std::string, bool, float, int, long>;
	BOOST_REQUIRE_EQUAL( pack_pos, 3 );
}

BOOST_AUTO_TEST_CASE( daw_pack_index_of_002 ) {
	constexpr auto const pack_pos =
	  daw::pack_index_of_v<double, std::string, bool, float, int, long>;
	BOOST_REQUIRE_EQUAL( pack_pos, 5 );
}

BOOST_AUTO_TEST_CASE( daw_pack_index_of_003 ) {
	constexpr auto const pack_pos =
	  daw::pack_index_of_v<bool, std::string, bool, float, int, long>;
	BOOST_REQUIRE_EQUAL( pack_pos, 1 );
}

BOOST_AUTO_TEST_CASE( daw_read_file_001 ) {
	auto f = daw::read_file( "./daw_utility_test_bin" );
	std::cout << f.size( ) << '\n';
}

/*
BOOST_AUTO_TEST_CASE( daw_get_pack_value_001 ) {
  constexpr auto const tst = daw::daw_get_pack_value( 2, "a", true, 4 );

  static_assert( daw::is_same_v<decltype( tst ), bool>, "Expected a bool" );
  BOOST_REQUIRE( tst );
}
*/

BOOST_AUTO_TEST_CASE( daw_pack_type_at_001 ) {
	using tst_t =
	  daw::pack_type_at<5, double, float, std::string, char, char *, int, void *>;

	constexpr bool tst = daw::is_same_v<int, tst_t>;

	std::cout << daw::pack_index_of_v<int, double, float, std::string, char,
	                                  char *, int, void *> << '\n';

	std::cout << typeid( tst_t ).name( ) << '\n';
	std::cout << typeid( int ).name( ) << '\n';
	std::cout << tst << '\n';
	BOOST_REQUIRE( tst );
}

BOOST_AUTO_TEST_CASE( daw_overload_001 ) {
	struct A {
		auto operator( )( int64_t a ) const {
			std::cout << "A->int\n";
			return 0;
		}
	};
	struct B {
		auto operator( )( std::string b ) const {
			std::cout << "B->std::string\n";
			return 1;
		}
	};
	struct C {
		auto operator( )( char *c ) const {
			std::cout << "D->bool\n";
			return 2;
		}
	};

	auto fn = daw::overload( A{}, B{}, C{} );
/*
	BOOST_REQUIRE_EQUAL( fn( 1 ), 0 );
	BOOST_REQUIRE_EQUAL( fn( std::string{"Hello"} ), 1 );
	BOOST_REQUIRE_EQUAL( fn( "Good bye" ), 2 );
	*/
}

int string_len( std::string const & str ) noexcept {
	std::cout << "string\n";
	return str.size( );
}

BOOST_AUTO_TEST_CASE( daw_overload_002 ) {
	auto fn = daw::overload(
	  []( int i ) noexcept {
		  std::cout << "int\n";
		  return log10( i );
	  },
	  string_len );

	BOOST_REQUIRE_EQUAL( fn( 10 ), 1 );
	BOOST_REQUIRE_EQUAL( fn( "hello" ), 5 );
}

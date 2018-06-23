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

#include "boost_test.h"
#include <sstream>
#include <string>

#include "daw_exception.h"
#include "daw_math.h"
#include "daw_parse_to.h"

BOOST_AUTO_TEST_CASE( daw_parse_to_001 ) {
	constexpr auto vals =
	  daw::parser::parse_to<int, int, int, int>( "0,1,2,3", "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<1>( vals ), 1 );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}

BOOST_AUTO_TEST_CASE( daw_parse_to_002 ) {
	auto vals = daw::parser::parse_to<int, std::string, int, int>(
	  "0,\"hello there\",2,3", "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<std::string>( vals ), "hello there" );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}

BOOST_AUTO_TEST_CASE( daw_parse_to_003 ) {
	constexpr daw::string_view const str = R"(0,"hello there",2,3)";
	constexpr auto vals =
	  daw::parser::parse_to<int, daw::string_view, int, int>( str, "," );
	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), 0 );
	BOOST_REQUIRE_EQUAL( std::get<daw::string_view>( vals ), "hello there" );
	BOOST_REQUIRE_EQUAL( std::get<2>( vals ), 2 );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), 3 );
}

namespace daw_values_from_stream_004_ns {
	struct X {
		int a;
		int64_t b;
		uint64_t c;
	};

	BOOST_AUTO_TEST_CASE( daw_parse_to_004 ) {
		constexpr auto x =
		  daw::construct_from<X, int, int64_t, uint64_t>( "1,-14334,3434234", "," );
		BOOST_REQUIRE_EQUAL( x.a, 1 );
		BOOST_REQUIRE_EQUAL( x.b, -14334 );
		BOOST_REQUIRE_EQUAL( x.c, 3434234 );
	}
} // namespace daw_values_from_stream_004_ns

BOOST_AUTO_TEST_CASE( daw_parse_to_005 ) {
	auto f = []( int a, int b, int c ) { return a + b + c; };
	auto result = daw::apply_string2( f, "1,2,3", "," );
	BOOST_REQUIRE_EQUAL( result, 6 );
}

BOOST_AUTO_TEST_CASE( daw_parse_to_006 ) {
	auto f = []( int a, int b, int c ) { return a + b + c; };
	auto result = daw::apply_string<int, int, int>( f, "1,2,3", "," );
	BOOST_REQUIRE_EQUAL( result, 6 );
}

BOOST_AUTO_TEST_CASE( daw_parse_to_007 ) {
	auto f = []( int a, int b, int c ) { return a + b + c; };
	auto result =
	  daw::apply_string2( f, "1	2  3", daw::parser::whitespace_splitter{} );
	BOOST_REQUIRE_EQUAL( result, 6 );
}

BOOST_AUTO_TEST_CASE( daw_parse_to_008 ) {
	auto f = []( int a, int b, int c ) { return a + b + c; };
	auto result = daw::apply_string<int, int, int>(
	  f, "1  2     3", daw::parser::whitespace_splitter{} );
	BOOST_REQUIRE_EQUAL( result, 6 );
}

BOOST_AUTO_TEST_CASE( daw_values_from_stream_001 ) {
	std::string str = "this 1 1.234 test";
	std::stringstream ss{str};
	auto vals =
	  daw::values_from_stream<daw::parser::converters::unquoted_string, int,
	                          float, daw::parser::converters::unquoted_string>(
	    ss, " " );

	BOOST_REQUIRE_EQUAL( std::get<0>( vals ), "this" );
	BOOST_REQUIRE_EQUAL( std::get<1>( vals ), 1 );
	BOOST_REQUIRE( daw::math::nearly_equal( std::get<2>( vals ), 1.234f ) );
	BOOST_REQUIRE_EQUAL( std::get<3>( vals ), "test" );
}

enum class e_colours : int { red = 2, green = 4, blue = 8 };

struct invalid_e_colour_exception {};

constexpr e_colours parse_to_value( daw::string_view str, daw::tag<e_colours> ) {
	daw::exception::daw_throw_on_true(
	  str.empty( ), "Attempt to parse an e_colour from an empty string_view" );
	switch( str.front( ) ) {
	case 'r':
	case 'R':
		return e_colours::red;
	case 'g':
	case 'G':
		return e_colours::green;
	case 'b':
	case 'B':
		return e_colours::blue;
	default:
		throw invalid_e_colour_exception{};
	}
}

namespace daw_parse_to_enum_001_ns {

	constexpr int get_value( e_colours colour ) noexcept {
		return static_cast<int>( colour );
	}

	constexpr int sum_colours( e_colours a, e_colours b ) noexcept {
		return get_value( a ) + get_value( b );
	}

	BOOST_AUTO_TEST_CASE( daw_parse_to_enum_001 ) {
		auto result = daw::apply_string<e_colours, e_colours>(
		  []( e_colours a, e_colours b ) { return sum_colours( a, b ); },
		  "green blue", daw::parser::whitespace_splitter{} );
		BOOST_REQUIRE_EQUAL( result, 12 );
	}

	struct callable_t {
		constexpr int operator( )( e_colours a, e_colours b ) const noexcept {
			return sum_colours( a, b );
		}
	};

	BOOST_AUTO_TEST_CASE( daw_parse_to_enum_002 ) {

		constexpr auto result = daw::apply_string<e_colours, e_colours>(
		  callable_t( ), "green blue", " " );

		BOOST_REQUIRE_EQUAL( result, 12 );
	}

	BOOST_AUTO_TEST_CASE( daw_parse_to_enum_003 ) {
		auto result = daw::apply_string<e_colours, e_colours, int>(
		  []( e_colours a, e_colours b, int c ) { return sum_colours( a, b ) + c; },
		  "green blue 534", daw::parser::whitespace_splitter{} );
		BOOST_REQUIRE_EQUAL( result, 546 );
	}

	struct ClassTest {
		int value = 0;
		constexpr ClassTest( ) noexcept = delete;
		constexpr ClassTest( int v ) noexcept
		  : value( v ) {}
	};

	constexpr ClassTest parse_to_value( daw::string_view str, daw::tag<ClassTest> ) {
		return daw::construct_from<ClassTest, int>( str );
	}

	BOOST_AUTO_TEST_CASE( daw_parse_to_class_001 ) {
		constexpr auto result = daw::apply_string<e_colours, ClassTest>(
		  []( e_colours a, ClassTest b ) {
			  return static_cast<int>( a ) + b.value;
		  },
		  "green 54", daw::parser::whitespace_splitter{} );
		BOOST_REQUIRE_EQUAL( result, 58 );
	}
} // namespace daw_parse_to_enum_001_ns

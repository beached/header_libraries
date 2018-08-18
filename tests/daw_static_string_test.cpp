// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <boost/utility/string_view.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_static_string.h"

#define FALSE( b ) ( !( b ) )
namespace daw {
	enum class tmp_e { a, b, c };

	constexpr bool is_equal_nc( daw::string_view lhs,
	                            daw::string_view rhs ) noexcept {
		if( lhs.size( ) != rhs.size( ) ) {
			return false;
		}
		bool result = true;
		for( size_t n = 0; n < lhs.size( ); ++n ) {
			result &= ( lhs[n] | ' ' ) == ( rhs[n] | ' ' );
		}
		return result;
	}

	tmp_e tmp_e_from_str( daw::string_view str ) {
		if( is_equal_nc( str, "a" ) ) {
			return tmp_e::a;
		}
		if( is_equal_nc( str, "b" ) ) {
			return tmp_e::b;
		}
		if( is_equal_nc( str, "c" ) ) {
			return tmp_e::c;
		}
		throw std::runtime_error( "unknown http request method" );
	}

	daw::static_string do_something( daw::static_string str, tmp_e &result ) {
		str = str.substr( 0, str.find_first_of( ' ' ) );
		result = tmp_e_from_str( str );
		return str;
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_contexpr_001 ) {
		daw::static_string a = "A test";
		tmp_e result = tmp_e::b;
		auto str = do_something( a, result );
		BOOST_REQUIRE( result == tmp_e::a );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_make_static_string_it ) {
		std::string a = "This is a test";
		auto b = daw::make_static_string_it( a.begin( ), a.end( ) );

		BOOST_REQUIRE_MESSAGE(
		  std::equal( a.begin( ), a.end( ), b.begin( ), b.end( ) ),
		  "string and static_string should be equal" );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_make_static_string_vector ) {
		std::string a = "This is a test";
		std::vector<char> b;
		std::copy( a.begin( ), a.end( ), std::back_inserter( b ) );

		auto c = daw::make_static_string( b );

		BOOST_REQUIRE_MESSAGE(
		  std::equal( a.begin( ), a.end( ), b.begin( ), b.end( ) ),
		  "string and vector should be equal" );
		BOOST_REQUIRE_MESSAGE(
		  std::equal( c.begin( ), c.end( ), b.begin( ), b.end( ) ),
		  "static_string and vector should be equal" );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_find_last_of_001 ) {
		static daw::static_string const a = "abcdefghijklm";
		std::string const b = "abcdefghijklm";
		static boost::string_view const c = "abcdefghijklm";
		auto const pos = a.find_last_of( "ij" );
		auto const pos2 = b.find_last_of( "ij" );
		auto const pos3 = c.find_last_of( "ij" );
		BOOST_REQUIRE_EQUAL( pos, pos2 );
		BOOST_REQUIRE_EQUAL( pos, pos3 );

		auto const es = a.find_last_of( "lm" );
		auto const es2 = b.find_last_of( "lm" );
		auto const es3 = c.find_last_of( "lm" );
		BOOST_REQUIRE_EQUAL( es, es2 );
		BOOST_REQUIRE_EQUAL( es, es3 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_make_test_001 ) {
		unsigned char const p[] = {'H', 'e', 'l', 'l', 'o', 0};
		auto sv =
		  daw::make_static_string_it( reinterpret_cast<char const *>( p ),
		                              reinterpret_cast<char const *>( p ) + 5 );
		daw::static_string p2 = "Hello";
		BOOST_REQUIRE_EQUAL( sv, p2 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_find_first_of_if_001 ) {
		daw::static_string const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) { return c == 'c'; } );
		BOOST_REQUIRE_EQUAL( pos, 2 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_find_first_not_of_if_001 ) {
		daw::static_string const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) { return c < 'c'; } );
		BOOST_REQUIRE_EQUAL( pos, 2 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_find_first_of_001 ) {
		daw::static_string const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def" );
		BOOST_REQUIRE_EQUAL( pos, 3 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_find_first_not_of_001 ) {
		daw::static_string const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc" );
		BOOST_REQUIRE_EQUAL( pos, 5 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_search_001 ) {
		daw::static_string const a = "abcdeaaaijklm";
		auto pos = a.search( "aaa" );
		BOOST_REQUIRE_EQUAL( pos, 5 );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_search_last_001 ) {
		daw::static_string const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa" );
		BOOST_REQUIRE_EQUAL( pos, 5 );
	}

	BOOST_AUTO_TEST_CASE( tc001 ) {
		daw::static_string view;
		BOOST_TEST_MESSAGE( "Constructs an empty string" );

		{ BOOST_REQUIRE( view.empty( ) ); }
		BOOST_TEST_MESSAGE( "Is 0 bytes in size" );

		{ BOOST_REQUIRE( view.size( ) == 0 ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc002 ) {
		std::string str = "Hello world";
		daw::static_string view = str;

		BOOST_TEST_MESSAGE( "Constructs a non-empty string" );
		{ BOOST_REQUIRE( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Has non-zero size" );
		{ BOOST_REQUIRE( view.size( ) != 0 ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc003 ) {
		auto empty_str = "";
		auto non_empty_str = "Hello World";

		BOOST_TEST_MESSAGE( "Is empty with empty string" );
		{
			daw::static_string view = empty_str;

			BOOST_REQUIRE( view.empty( ) );
		}

		BOOST_TEST_MESSAGE( "Is non-empty with non-empty string" );
		{
			daw::static_string view = non_empty_str;

			BOOST_REQUIRE( FALSE( view.empty( ) ) );
		}

		BOOST_TEST_MESSAGE( "Is size 0 with empty string" );
		{
			daw::static_string view = empty_str;

			BOOST_REQUIRE( view.size( ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Is not size 0 with non-empty string" );
		{
			daw::static_string view = non_empty_str;

			BOOST_REQUIRE( view.size( ) != 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc004 ) {
		daw::static_string empty = "";
		daw::static_string view = "Hello world";

		BOOST_TEST_MESSAGE( "Is empty with empty string" );
		{ BOOST_REQUIRE( empty.empty( ) ); }

		BOOST_TEST_MESSAGE( "Is non-empty with non-empty string" );
		{ BOOST_REQUIRE( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Is size 0 with empty string" );
		{ BOOST_REQUIRE( empty.size( ) == 0 ); }

		BOOST_TEST_MESSAGE( "Is not size 0 with non-empty string" );
		{ BOOST_REQUIRE( view.size( ) != 0 ); }
	}

	//----------------------------------------------------------------------------
	// Capacity
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc004capacity ) {
		const char *str = "Hello World";
		daw::static_string view = str;

		BOOST_TEST_MESSAGE( "Returns non-zero for non-empty string" );
		{ BOOST_REQUIRE( view.size( ) == std::char_traits<char>::length( str ) ); }

		BOOST_TEST_MESSAGE( "Returns 0 for empty string" );
		{
			view = "";

			BOOST_REQUIRE( view.size( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc005capacity ) {
		const char *str = "Hello World";
		daw::static_string view = str;

		BOOST_TEST_MESSAGE( "Returns non-zero for non-empty string" );
		{
			BOOST_REQUIRE( view.length( ) == std::char_traits<char>::length( str ) );
		}

		BOOST_TEST_MESSAGE( "Returns 0 for empty string" );
		{
			view = "";

			BOOST_REQUIRE( view.length( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc006capacity ) {
		const char *str = "Hello World";
		daw::static_string view = str;

		BOOST_TEST_MESSAGE( "Returns false on non-empty string" );
		{ BOOST_REQUIRE( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Returns true on empty string" );
		{
			view = "";

			BOOST_REQUIRE( view.empty( ) );
		}
	}

	//----------------------------------------------------------------------------
	// Element Access
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc011accessor ) {
		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns reference to first character" );
		{ BOOST_REQUIRE( view.front( ) == 'H' ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc012accessor ) {
		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns reference to last character" );
		{ BOOST_REQUIRE( view.back( ) == 'd' ); }
	}

	//----------------------------------------------------------------------------
	// Modifiers
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc014modifier ) {

		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Removes last 6 characters" );
		{
			view.remove_suffix( 6 );

			BOOST_REQUIRE_EQUAL( view, "Hello" );
			BOOST_REQUIRE( view == "Hello" );
		}
	}

	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// String Operations
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc016conversion ) {
		daw::static_string view = "Hello World";

		std::string string = view.to_string( );

		BOOST_TEST_MESSAGE( "Copies view to new location in std::string" );
		{ BOOST_REQUIRE( view.data( ) != string.data( ) ); }

		BOOST_TEST_MESSAGE( "Copied string contains same contents as view" );
		{ BOOST_REQUIRE( string == "Hello World" ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc017conversion ) {
		daw::static_string view = "Hello World";

		std::string string = static_cast<std::string>( view );

		BOOST_TEST_MESSAGE( "Copies view to new location in std::string" );
		{ BOOST_REQUIRE( view.data( ) != string.data( ) ); }

		BOOST_TEST_MESSAGE( "Copied string contains same contents as view" );
		{ BOOST_REQUIRE( string == "Hello World" ); }
	}

	//----------------------------------------------------------------------------
	// Operations
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc018operation ) {
		daw::static_string const view = "Hello World";

		BOOST_TEST_MESSAGE( "Throws std::out_of_range if pos >= view.size()" );
		{
			char result[11];

			BOOST_REQUIRE_THROW( view.copy( result, 11, 11 ), std::out_of_range );
		}

		BOOST_TEST_MESSAGE( "Copies entire string" );
		{
			char result[11];
			view.copy( result, 11 );

			BOOST_REQUIRE( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Copies remaining characters if count > size" );
		{
			char result[11];
			view.copy( result, 20 );

			BOOST_REQUIRE( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Copies part of the string" );
		{
			char result[11];
			view.copy( result, 5 );

			BOOST_REQUIRE( std::strncmp( result, "Hello", 5 ) == 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Copies part of the string, offset from the beginning" );
		{
			char result[11];
			view.copy( result, 10, 6 );

			BOOST_REQUIRE( std::strncmp( result, "World", 5 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Returns number of characters copied" );
		{
			char result[11];

			BOOST_REQUIRE( view.copy( result, 20 ) == 11 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc019operation ) {
		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns the full string when given no args" );
		{
			auto substr = view.substr( );
			BOOST_REQUIRE( substr == "Hello World" );
		}

		BOOST_TEST_MESSAGE( "Returns last part of string" );
		{
			auto substr = view.substr( 6 );
			BOOST_REQUIRE( substr == "World" );
		}

		BOOST_TEST_MESSAGE( "Substring returns at most count characters" );
		{
			auto substr = view.substr( 6, 1 );
			BOOST_REQUIRE_EQUAL( substr, "W" );
			BOOST_REQUIRE( substr == "W" );
		}

		BOOST_TEST_MESSAGE( "Returns up to end of string if length > size" );
		{
			auto substr = view.substr( 6, 10 );
			BOOST_REQUIRE( substr == "World" );
		}

		BOOST_TEST_MESSAGE( "Throws std::out_of_range if pos > size" );
		{ BOOST_REQUIRE_THROW( view.substr( 15 ), std::out_of_range ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc020comparison ) {
		BOOST_TEST_MESSAGE( "Returns 0 for identical views" );
		{
			daw::static_string view = "Hello World";

			BOOST_REQUIRE( view.compare( "Hello World" ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Returns nonzero for different views" );
		{
			daw::static_string view = "Hello World";

			BOOST_REQUIRE( view.compare( "Goodbye World" ) != 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns > 0 for substring beginning with same string" );
		{
			daw::static_string view = "Hello World";

			BOOST_REQUIRE( view.compare( "Hello" ) > 0 );
		}

		BOOST_TEST_MESSAGE( "Returns < 0 for superstring beginning with self" );
		{
			daw::static_string view = "Hello";

			BOOST_REQUIRE( view.compare( "Hello World" ) < 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns < 0 for same-sized string compared to character greater than "
		  "char" );
		{
			daw::static_string view = "1234567";

			BOOST_REQUIRE( view.compare( "1234667" ) < 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns > 0 for same-sized string compared to character less than "
		  "char" );
		{
			daw::static_string view = "1234567";

			BOOST_REQUIRE( view.compare( "1234467" ) > 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc021comparison ) {
		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Is equal" );
		{
			BOOST_TEST_MESSAGE( "Returns true for equal string views" );
			{
				daw::static_string view2 = "Hello World";

				BOOST_REQUIRE( view == view2 );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with left char array" );
			{ BOOST_REQUIRE( "Hello World" == view ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right char array" );
			{ BOOST_REQUIRE( view == "Hello World" ); }

			BOOST_TEST_MESSAGE( "Returns true for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				BOOST_REQUIRE( str == view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				BOOST_REQUIRE( view == str );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				BOOST_REQUIRE( str == view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				BOOST_REQUIRE( view == str );
			}
		}

		BOOST_TEST_MESSAGE( "Is not equal" );
		{
			BOOST_TEST_MESSAGE( "Returns false for non-equal string views" );
			{
				daw::static_string view2 = "Goodbye World";

				BOOST_REQUIRE( FALSE( view == view2 ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left char array" );
			{ BOOST_REQUIRE( FALSE( ( "Goodbye World" == view ) ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right char array" );
			{ BOOST_REQUIRE( FALSE( ( view == "Goodbye World" ) ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				BOOST_REQUIRE( FALSE( str == view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				BOOST_REQUIRE( FALSE( view == str ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				BOOST_REQUIRE( FALSE( str == view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				BOOST_REQUIRE( FALSE( view == str ) );
			}
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc022comparison ) {
		daw::static_string view = "Hello World";

		BOOST_TEST_MESSAGE( "Is equal" );
		{
			BOOST_TEST_MESSAGE( "Returns false for equal string views" );
			{
				daw::static_string view2 = "Hello World";

				BOOST_REQUIRE( FALSE( view != view2 ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left char array" );
			{ BOOST_REQUIRE( FALSE( "Hello World" != view ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right char array" );
			{ BOOST_REQUIRE( FALSE( view != "Hello World" ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				BOOST_REQUIRE( FALSE( str != view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				BOOST_REQUIRE( FALSE( view != str ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				BOOST_REQUIRE( FALSE( str != view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				BOOST_REQUIRE( FALSE( view != str ) );
			}
		}

		BOOST_TEST_MESSAGE( "Is not equal" );
		{
			BOOST_TEST_MESSAGE( "Returns true for non-equal string views" );
			{
				daw::static_string view2 = "Goodbye World";

				BOOST_REQUIRE( view != view2 );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left char array" );
			{ BOOST_REQUIRE( "Goodbye World" != view ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right char array" );
			{ BOOST_REQUIRE( view != "Goodbye World" ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				BOOST_REQUIRE( str != view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				BOOST_REQUIRE( view != str );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				BOOST_REQUIRE( str != view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				BOOST_REQUIRE( view != str );
			}
		}
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_split_001 ) {
		daw::static_string str = "This is a test of the split";
		auto const str_splt = split( str, ' ' );
		BOOST_REQUIRE_EQUAL( str_splt.size( ), 7 );
		std::cout << str << "\n\n";
		std::cout << "items:\n";
		for( auto const &s : str_splt ) {
			std::cout << '"' << s << "\"\n";
		}
		std::cout << "\n\n";
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_split_005 ) {
		std::string b_str;
		daw::static_string b = b_str;
		auto const str_splt = split( b, "," );
		BOOST_REQUIRE( str_splt.empty( ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_001 ) {
		BOOST_REQUIRE( daw::can_be_static_string<decltype( "Hello" )> );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_001 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.starts_with( "This" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_002 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.starts_with(
		  daw::static_string{"This"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_003 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.starts_with( 'T' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_004 ) {
		BOOST_REQUIRE(
		  !daw::static_string{"This is a test"}.starts_with( "ahis" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_005 ) {
		BOOST_REQUIRE( !daw::static_string{"This is a test"}.starts_with(
		  daw::static_string{"ahis"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_starts_with_006 ) {
		BOOST_REQUIRE( !daw::static_string{"This is a test"}.starts_with( 'a' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_001 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.ends_with( "test" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_002 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.ends_with(
		  daw::static_string{"test"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_003 ) {
		BOOST_REQUIRE( daw::static_string{"This is a test"}.ends_with( 't' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_004 ) {
		BOOST_REQUIRE( !daw::static_string{"This is a test"}.ends_with( "aest" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_005 ) {
		BOOST_REQUIRE( !daw::static_string{"This is a test"}.ends_with(
		  daw::static_string{"aest"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_static_string_ends_with_006 ) {
		BOOST_REQUIRE( !daw::static_string{"This is a test"}.ends_with( 'a' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_constexpr_001 ) {
		constexpr daw::static_string a = "This is a test";
	}

	BOOST_AUTO_TEST_CASE( daw_static_string_overfull_001 ) {
		daw::basic_static_string<char, 4> a = "This";
		BOOST_CHECK_THROW( a.push_back( 'a' ), std::out_of_range );
	}

	constexpr void cxcopy_test( daw::static_string &lhs,
	                            static_string const &rhs ) noexcept {
		lhs = rhs;
	}

	constexpr auto cxcopy_cosnt_test( static_string const &rhs ) noexcept {
		return rhs;
	}

	constexpr auto cxcopy_tester( ) noexcept {
		daw::static_string a = "String a";
		daw::static_string b = "String b";
		cxcopy_test( a, b );
		return cxcopy_cosnt_test( a );
	}

	BOOST_AUTO_TEST_CASE( constexpr_copy_001 ) {
		constexpr auto str = cxcopy_tester( );
		BOOST_REQUIRE_EQUAL( str, "String b" );
	}
} // namespace daw

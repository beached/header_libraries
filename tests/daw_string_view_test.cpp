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

#include <boost/utility/string_view.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#ifndef NOSTRING
#include <string>
#endif
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_string_view.h"

#define FALSE( b ) ( !( b ) )
namespace daw {
#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( daw_string_view_test_001 ) {
		constexpr daw::string_view const a = "This is a test";
		std::cout << a.to_string( ) << '\n';
		std::string b_str = "Testing again" + a;
		daw::string_view b = b_str;

		auto c = a;
		c.remove_prefix( );
		std::cout << c << ' ' << c.front( ) << ' ' << c[3] << '\n';
		c = a;
		c = b;
		std::cout << c << ' ' << c.front( ) << ' ' << c[3] << '\n';
	}
#endif

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

	daw::string_view do_something( daw::string_view str, tmp_e &result ) {
		str = str.substr( 0, str.find_first_of( ' ' ) );
		result = tmp_e_from_str( str );

		return str;
	}
	BOOST_AUTO_TEST_CASE( daw_string_view_contexpr_001 ) {
		daw::string_view a = "A test";
		tmp_e result = tmp_e::b;
		auto str = do_something( a, result );
		::Unused( str );
		daw::expecting( result == tmp_e::a );
	}

#ifndef HASNOSTRING
	BOOST_AUTO_TEST_CASE( daw_string_view_make_string_view_it ) {
		std::string a = "This is a test";
		auto b = daw::make_string_view_it( a.begin( ), a.end( ) );

		daw::expecting_message(
		  std::equal( a.begin( ), a.end( ), b.begin( ), b.end( ) ),
		  "string and string_view should be equal" );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_make_string_view_vector ) {
		std::string a = "This is a test";
		std::vector<char> b;
		std::copy( a.begin( ), a.end( ), std::back_inserter( b ) );

		auto c = daw::make_string_view( b );

		daw::expecting_message(
		  std::equal( a.begin( ), a.end( ), b.begin( ), b.end( ) ),
		  "string and vector should be equal" );
		daw::expecting_message(
		  std::equal( c.begin( ), c.end( ), b.begin( ), b.end( ) ),
		  "string_view and vector should be equal" );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_last_of_001 ) {
		static daw::string_view const a = "abcdefghijklm";
		std::string const b = "abcdefghijklm";
		static boost::string_view const c = "abcdefghijklm";
		auto const pos = a.find_last_of( "ij" );
		auto const pos2 = b.find_last_of( "ij" );
		auto const pos3 = c.find_last_of( "ij" );
		daw::expecting( pos, pos2 );
		daw::expecting( pos, pos3 );

		auto const es = a.find_last_of( "lm" );
		auto const es2 = b.find_last_of( "lm" );
		auto const es3 = c.find_last_of( "lm" );
		daw::expecting( es, es2 );
		daw::expecting( es, es3 );
	}
#endif

	BOOST_AUTO_TEST_CASE( daw_string_view_make_test_001 ) {
		unsigned char const p[] = {'H', 'e', 'l', 'l', 'o', 0};
		auto sv =
		  daw::make_string_view_it( reinterpret_cast<char const *>( p ),
		                            reinterpret_cast<char const *>( p ) + 5 );
		daw::string_view p2 = "Hello";
		daw::expecting( sv, p2 );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_first_of_if_001 ) {
		daw::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) { return c == 'c'; } );
		daw::expecting( 2U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_first_not_of_if_001 ) {
		daw::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) { return c < 'c'; } );
		daw::expecting( 2U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_first_of_001 ) {
		daw::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def" );
		daw::expecting( 3U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_first_not_of_001 ) {
		daw::string_view const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc" );
		daw::expecting( 5U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_find_last_not_of_001 ) {
		daw::string_view const a = "abcabfghijklm";
		auto pos = a.find_last_not_of( "abc" );
		daw::expecting( 3U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_search_001 ) {
		daw::string_view const a = "abcdeaaaijklm";
		auto pos = a.search( "aaa" );
		daw::expecting( 5U, pos );
	}

	BOOST_AUTO_TEST_CASE( daw_string_view_search_last_001 ) {
		daw::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa" );
		daw::expecting( 5U, pos );
	}

	BOOST_AUTO_TEST_CASE( tc001 ) {
		daw::string_view view;
		BOOST_TEST_MESSAGE( "Constructs an empty string" );

		{ daw::expecting( view.empty( ) ); }
		BOOST_TEST_MESSAGE( "Is 0 bytes in size" );

		{ daw::expecting( view.size( ) == 0 ); }

		BOOST_TEST_MESSAGE( "Points to null" );

		{ daw::expecting( view.data( ) == nullptr ); }
	}

	//----------------------------------------------------------------------------

#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( tc002 ) {
		std::string str = "Hello world";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Constructs a non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Has non-zero size" );
		{ daw::expecting( view.size( ) != 0 ); }

		BOOST_TEST_MESSAGE( "Points to original string" );
		{ daw::expecting( view.data( ) == str.data( ) ); }
	}
#endif
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc003 ) {
		auto empty_str = "";
		auto non_empty_str = "Hello World";

		BOOST_TEST_MESSAGE( "Is empty with empty string" );
		{
			daw::string_view view = empty_str;

			daw::expecting( view.empty( ) );
		}

		BOOST_TEST_MESSAGE( "Is non-empty with non-empty string" );
		{
			daw::string_view view = non_empty_str;

			daw::expecting( FALSE( view.empty( ) ) );
		}

		BOOST_TEST_MESSAGE( "Is size 0 with empty string" );
		{
			daw::string_view view = empty_str;

			daw::expecting( view.size( ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Is not size 0 with non-empty string" );
		{
			daw::string_view view = non_empty_str;

			daw::expecting( view.size( ) != 0 );
		}

		BOOST_TEST_MESSAGE( "Points to original string" );
		{
			daw::string_view view = non_empty_str;

			daw::expecting( view.data( ) == non_empty_str );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc004 ) {
		daw::string_view empty = "";
		daw::string_view view = "Hello world";

		BOOST_TEST_MESSAGE( "Is empty with empty string" );
		{ daw::expecting( empty.empty( ) ); }

		BOOST_TEST_MESSAGE( "Is non-empty with non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Is size 0 with empty string" );
		{ daw::expecting( empty.size( ) == 0 ); }

		BOOST_TEST_MESSAGE( "Is not size 0 with non-empty string" );
		{ daw::expecting( view.size( ) != 0 ); }
	}

	//----------------------------------------------------------------------------
	// Capacity
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc004capacity ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Returns non-zero for non-empty string" );
		{ daw::expecting( view.size( ) == std::char_traits<char>::length( str ) ); }

		BOOST_TEST_MESSAGE( "Returns 0 for empty string" );
		{
			view = "";

			daw::expecting( view.size( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc005capacity ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Returns non-zero for non-empty string" );
		{
			daw::expecting( view.length( ) == std::char_traits<char>::length( str ) );
		}

		BOOST_TEST_MESSAGE( "Returns 0 for empty string" );
		{
			view = "";

			daw::expecting( view.length( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc006capacity ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Returns false on non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		BOOST_TEST_MESSAGE( "Returns true on empty string" );
		{
			view = "";

			daw::expecting( view.empty( ) );
		}
	}

	//----------------------------------------------------------------------------
	// Element Access
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc007accessor ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Points to original data source" );
		{ daw::expecting( view.c_str( ) == str ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc008accessor ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Points to original data source" );
		{ daw::expecting( view.data( ) == str ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc009accessor ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Returns reference to entry at position" );
		{ daw::expecting( &view[0] == str ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc010accessor ) {
		const char *str = "Hello World";
		daw::string_view view = str;

		BOOST_TEST_MESSAGE( "Returns reference to entry at position" );
		{ daw::expecting( &view.at( 0 ) == str ); }

		BOOST_TEST_MESSAGE( "Throws when out of range" );
		{ BOOST_REQUIRE_THROW( view.at( 11 ), std::out_of_range ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc011accessor ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns reference to first character" );
		{ daw::expecting( view.front( ) == 'H' ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc012accessor ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns reference to last character" );
		{ daw::expecting( view.back( ) == 'd' ); }
	}

	//----------------------------------------------------------------------------
	// Modifiers
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc013modifier ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Removes first n characters" );
		{
			view.remove_prefix( 6 );

			daw::expecting( ( view == "World" ) );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc014modifier ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Removes last n characters" );
		{
			view.remove_suffix( 6 );

			daw::expecting( view, "Hello" );
			daw::expecting( view == "Hello" );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc015modifier ) {
		const char *str1 = "Hello World";
		const char *str2 = "Goodbye World";
		daw::string_view view1 = str1;
		daw::string_view view2 = str2;

		BOOST_TEST_MESSAGE( "Swaps entries" );
		{
			view1.swap( view2 );

			daw::expecting(
			  ( ( view1.data( ) == str2 ) && ( view2.data( ) == str1 ) ) );
		}
	}

	//----------------------------------------------------------------------------
	// String Operations
	//----------------------------------------------------------------------------
#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( tc016conversion ) {
		daw::string_view view = "Hello World";

		std::string string = view.to_string( );

		BOOST_TEST_MESSAGE( "Copies view to new location in std::string" );
		{ daw::expecting( view.data( ) != string.data( ) ); }

		BOOST_TEST_MESSAGE( "Copied string contains same contents as view" );
		{ daw::expecting( string == "Hello World" ); }
	}
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc017conversion ) {
		daw::string_view view = "Hello World";

		std::string string = static_cast<std::string>( view );

		BOOST_TEST_MESSAGE( "Copies view to new location in std::string" );
		{ daw::expecting( view.data( ) != string.data( ) ); }

		BOOST_TEST_MESSAGE( "Copied string contains same contents as view" );
		{ daw::expecting( string == "Hello World" ); }
	}
#endif
	//----------------------------------------------------------------------------
	// Operations
	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc018operation ) {
		daw::string_view const view = "Hello World";

		BOOST_TEST_MESSAGE( "Throws std::out_of_range if pos >= view.size()" );
		{
			char result[11];

			BOOST_REQUIRE_THROW( view.copy( result, 11, 11 ), std::out_of_range );
		}

		BOOST_TEST_MESSAGE( "Copies entire string" );
		{
			char result[11];
			view.copy( result, 11 );

			daw::expecting( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Copies remaining characters if count > size" );
		{
			char result[11];
			view.copy( result, 20 );

			daw::expecting( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Copies part of the string" );
		{
			char result[11];
			view.copy( result, 5 );

			daw::expecting( std::strncmp( result, "Hello", 5 ) == 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Copies part of the string, offset from the beginning" );
		{
			char result[11];
			view.copy( result, 10, 6 );

			daw::expecting( std::strncmp( result, "World", 5 ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Returns number of characters copied" );
		{
			char result[11];

			daw::expecting( view.copy( result, 20 ) == 11 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc019operation ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Returns the full string when given no args" );
		{
			auto substr = view.substr( );
			daw::expecting( substr == "Hello World" );
		}

		BOOST_TEST_MESSAGE( "Returns last part of string" );
		{
			auto substr = view.substr( 6 );
			daw::expecting( substr == "World" );
		}

		BOOST_TEST_MESSAGE( "Substring returns at most count characters" );
		{
			auto substr = view.substr( 6, 1 );
			daw::expecting( substr, "W" );
			daw::expecting( substr == "W" );
		}

		BOOST_TEST_MESSAGE( "Returns up to end of string if length > size" );
		{
			auto substr = view.substr( 6, 10 );
			daw::expecting( substr == "World" );
		}

		BOOST_TEST_MESSAGE( "Throws std::out_of_range if pos > size" );
		{ BOOST_REQUIRE_THROW( view.substr( 15 ), std::out_of_range ); }
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc020comparison ) {
		BOOST_TEST_MESSAGE( "Returns 0 for identical views" );
		{
			daw::string_view view = "Hello World";

			daw::expecting( view.compare( "Hello World" ) == 0 );
		}

		BOOST_TEST_MESSAGE( "Returns nonzero for different views" );
		{
			daw::string_view view = "Hello World";

			daw::expecting( view.compare( "Goodbye World" ) != 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns > 0 for substring beginning with same string" );
		{
			daw::string_view view = "Hello World";

			daw::expecting( view.compare( "Hello" ) > 0 );
		}

		BOOST_TEST_MESSAGE( "Returns < 0 for superstring beginning with self" );
		{
			daw::string_view view = "Hello";

			daw::expecting( view.compare( "Hello World" ) < 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns < 0 for same-sized string compared to character greater than "
		  "char" );
		{
			daw::string_view view = "1234567";

			daw::expecting( view.compare( "1234667" ) < 0 );
		}

		BOOST_TEST_MESSAGE(
		  "Returns > 0 for same-sized string compared to character less than "
		  "char" );
		{
			daw::string_view view = "1234567";

			daw::expecting( view.compare( "1234467" ) > 0 );
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc021comparison ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Is equal" );
		{
			BOOST_TEST_MESSAGE( "Returns true for equal string views" );
			{
				daw::string_view view2 = "Hello World";

				daw::expecting( view == view2 );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with left char array" );
			{ daw::expecting( "Hello World" == view ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right char array" );
			{ daw::expecting( view == "Hello World" ); }

			BOOST_TEST_MESSAGE( "Returns true for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( str == view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( view == str );
			}
#ifndef NOSTRING
			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( str == view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( view == str );
			}
#endif
		}

		BOOST_TEST_MESSAGE( "Is not equal" );
		{
			BOOST_TEST_MESSAGE( "Returns false for non-equal string views" );
			{
				daw::string_view view2 = "Goodbye World";

				daw::expecting( FALSE( view == view2 ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left char array" );
			{ daw::expecting( FALSE( ( "Goodbye World" == view ) ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right char array" );
			{ daw::expecting( FALSE( ( view == "Goodbye World" ) ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( FALSE( str == view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( FALSE( view == str ) );
			}

#ifndef NOSTRING
			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( FALSE( str == view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( FALSE( view == str ) );
			}

			BOOST_TEST_MESSAGE( "Two unequal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "2";
				daw::string_view sv1{str1};
				daw::string_view sv2{str2};
				daw::expecting( FALSE( sv1 == sv2 ) );
			}
			BOOST_TEST_MESSAGE( "Two equal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "1";
				daw::string_view sv1{str1};
				daw::string_view sv2{str2};
				daw::expecting( sv1, sv2 );
			}
#endif
		}
	}

	//----------------------------------------------------------------------------

	BOOST_AUTO_TEST_CASE( tc022comparison ) {
		daw::string_view view = "Hello World";

		BOOST_TEST_MESSAGE( "Is equal" );
		{
			BOOST_TEST_MESSAGE( "Returns false for equal string views" );
			{
				daw::string_view view2 = "Hello World";

				daw::expecting( FALSE( view != view2 ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left char array" );
			{ daw::expecting( FALSE( "Hello World" != view ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right char array" );
			{ daw::expecting( FALSE( view != "Hello World" ) ); }

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( FALSE( str != view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( FALSE( view != str ) );
			}

#ifndef NOSTRING
			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( FALSE( str != view ) );
			}

			BOOST_TEST_MESSAGE(
			  "Returns false for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( FALSE( view != str ) );
			}
#endif
		}

		BOOST_TEST_MESSAGE( "Is not equal" );
		{
			BOOST_TEST_MESSAGE( "Returns true for non-equal string views" );
			{
				daw::string_view view2 = "Goodbye World";

				daw::expecting( view != view2 );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left char array" );
			{ daw::expecting( "Goodbye World" != view ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right char array" );
			{ daw::expecting( view != "Goodbye World" ); }

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( str != view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( view != str );
			}

#ifndef NOSTRING
			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( str != view );
			}

			BOOST_TEST_MESSAGE(
			  "Returns true for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( view != str );
			}
#endif
		}
	}

#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( daw_string_view_split_001 ) {
		std::string str = "This is a test of the split";
		auto const str_splt = split( daw::string_view{str}, ' ' );
		daw::expecting( 7U, str_splt.size( ) );
		std::cout << str << "\n\n";
		std::cout << "items:\n";
		for( auto const &s : str_splt ) {
			std::cout << '"' << s << "\"\n";
		}
		std::cout << "\n\n";
	}
#endif

	BOOST_AUTO_TEST_CASE( daw_string_view_split_002 ) {
		char const str[] = "This is a test of the split";
		auto const str_splt = split( daw::string_view{str}, ' ' );
		daw::expecting( 7U, str_splt.size( ) );
	}

#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( daw_string_view_split_003 ) {
		std::string str = "This is a test of the split";
		auto const str_splt = split( str, []( char c ) { return c == ' '; } );
		daw::expecting( 7U, str_splt.size( ) );
	}
#endif
	BOOST_AUTO_TEST_CASE( daw_string_view_split_004 ) {
		char const str[] = "This is a test of the split";
		auto const str_splt = split( str, []( char c ) { return c == ' '; } );
		daw::expecting( 7U, str_splt.size( ) );
	}

#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( daw_string_view_split_005 ) {
		std::string b_str;
		daw::string_view b = b_str;
		auto const str_splt = split( b, ',' );
		daw::expecting( str_splt.empty( ) );
	}
#endif
	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_001 ) {
		daw::expecting( daw::can_be_string_view<decltype( "Hello" )> );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_001 ) {
		daw::expecting( daw::string_view{"This is a test"}.starts_with( "This" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_002 ) {
		daw::expecting( daw::string_view{"This is a test"}.starts_with(
		  daw::string_view{"This"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_003 ) {
		daw::expecting( daw::string_view{"This is a test"}.starts_with( 'T' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_004 ) {
		daw::expecting( !daw::string_view{"This is a test"}.starts_with( "ahis" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_005 ) {
		daw::expecting( !daw::string_view{"This is a test"}.starts_with(
		  daw::string_view{"ahis"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_starts_with_006 ) {
		daw::expecting( !daw::string_view{"This is a test"}.starts_with( 'a' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_001 ) {
		daw::expecting( daw::string_view{"This is a test"}.ends_with( "test" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_002 ) {
		daw::expecting( daw::string_view{"This is a test"}.ends_with(
		  daw::string_view{"test"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_003 ) {
		daw::expecting( daw::string_view{"This is a test"}.ends_with( 't' ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_004 ) {
		daw::expecting( !daw::string_view{"This is a test"}.ends_with( "aest" ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_005 ) {
		daw::expecting( !daw::string_view{"This is a test"}.ends_with(
		  daw::string_view{"aest"} ) );
	}

	BOOST_AUTO_TEST_CASE( daw_can_be_string_view_ends_with_006 ) {
		daw::expecting( !daw::string_view{"This is a test"}.ends_with( 'a' ) );
	}

#ifndef NOSTRING
	BOOST_AUTO_TEST_CASE( daw_to_string_view_001 ) {
		auto val = daw::to_string_view( std::to_string( 5 ) ).to_string( );
		daw::expecting( "5", val );
	}

	BOOST_AUTO_TEST_CASE( daw_pop_front_sv_test_001 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		daw::expecting( sv.pop_front( " " ), "This" );
		daw::expecting( sv.pop_front( " " ), "is" );
		daw::expecting( sv.pop_front( " " ), "a" );
		daw::expecting( sv.pop_front( " " ), "test" );
		daw::expecting( sv.empty( ) );
	}

	BOOST_AUTO_TEST_CASE( daw_pop_back_count_test_001 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		auto result = sv.pop_back( 4 );
		daw::expecting( result, "test" );
		daw::expecting( sv, "This is a " );
	}

	BOOST_AUTO_TEST_CASE( daw_pop_back_sv_test_001 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		daw::expecting( sv.pop_back( " " ), "test" );
		daw::expecting( sv.pop_back( " " ), "a" );
		daw::expecting( sv.pop_back( " " ), "is" );
		daw::expecting( sv.pop_back( " " ), "This" );
		daw::expecting( sv.empty( ) );
	}

	BOOST_AUTO_TEST_CASE( daw_pop_front_pred_test_001 ) {
		std::string str = "This is1a test";
		daw::string_view sv{str.data( ), str.size( )};
		auto lhs = sv.pop_front( []( auto c ) { return std::isdigit( c ); } );
		daw::expecting( lhs, "This is" );
		daw::expecting( sv, "a test" );
	}

	BOOST_AUTO_TEST_CASE( daw_pop_back_pred_test_001 ) {
		std::string str = "This is1a test";
		daw::string_view sv{str.data( ), str.size( )};
		auto rhs = sv.pop_back( []( auto c ) { return std::isdigit( c ); } );
		daw::expecting( sv, "This is" );
		daw::expecting( rhs, "a test" );
	}

	BOOST_AUTO_TEST_CASE( daw_try_pop_back_sv_test_001 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		daw::expecting( sv.try_pop_back( " " ), "test" );
		daw::expecting( sv.try_pop_back( " " ), "a" );
		daw::expecting( sv.try_pop_back( " " ), "is" );
		daw::expecting( sv.try_pop_back( " " ), "" );
		daw::expecting( sv, "This" );
	}

	BOOST_AUTO_TEST_CASE( daw_try_pop_back_sv_test_002 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		auto result = sv.try_pop_back( "blah" );
		daw::expecting( result.empty( ) );
		daw::expecting( sv == str );
	}

	BOOST_AUTO_TEST_CASE( daw_try_pop_front_sv_test_001 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		daw::expecting( sv.try_pop_front( " " ), "This" );
		daw::expecting( sv.try_pop_front( " " ), "is" );
		daw::expecting( sv.try_pop_front( " " ), "a" );
		daw::expecting( sv.try_pop_front( " " ) != "test" );
		daw::expecting( sv, "test" );
	}

	BOOST_AUTO_TEST_CASE( daw_try_pop_front_sv_test_002 ) {
		std::string str = "This is a test";
		daw::string_view sv{str.data( ), str.size( )};
		auto result = sv.try_pop_front( "blah" );
		daw::expecting( result.empty( ) );
		daw::expecting( sv == str );
		daw::string_view s{};
		s.remove_prefix( );
	}
#endif
	static_assert( basic_string_view( "This is a test" ).extent == 14 );

	template<size_t N>
	constexpr bool extent_test_001( char const ( &str )[N] ) noexcept {
		basic_string_view sv( str );
		return sv.extent >= 0;
	}
	static_assert( extent_test_001( "this is a test" ) );

	template<size_t N>
	constexpr bool extent_to_dynamic_001( char const ( &str )[N] ) noexcept {
		basic_string_view sv( str );
		string_view sv2{};
		sv2 = sv;
		return sv.extent >= 0 and sv2.extent == daw::dynamic_string_size;
	}
	static_assert( extent_to_dynamic_001( "Testing testing 1 2 3" ) );
} // namespace daw

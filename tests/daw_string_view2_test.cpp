// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#include <daw/daw_string_view2.h>
#include <daw/daw_utility.h>

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

static_assert( std::is_trivially_copyable_v<daw::sv2::string_view> );

#if defined( DAW_USE_EXCEPTIONS )
#define daw_expecting( Lhs, Rhs )                                        \
	do                                                                     \
		if( ( Lhs ) != ( Rhs ) ) {                                           \
			throw [] {                                                         \
				std::stringstream ss{ };                                         \
				ss << "Invalid result. Expecting '" #Lhs << " == " #Rhs << "'\n" \
				   << "File: " << __FILE__ << "\nLine: " << __LINE__;            \
				return std::logic_error( ss.str( ) );                            \
			}( );                                                              \
		}                                                                    \
	while( false )
#else
#define daw_expecting( Lhs, Rhs )                                             \
	do                                                                          \
		if( ( Lhs ) != ( Rhs ) ) {                                                \
			std::cerr << "Invalid result. Expecting '" #Lhs << " == " #Rhs << "'\n" \
			          << "File: " << __FILE__ << "\nLine: " << __LINE__             \
			          << std::endl;                                                 \
			std::abort( );                                                          \
		}                                                                         \
	while( false )
#endif

#if defined( DAW_USE_EXCEPTIONS )
#define daw_expecting_message( Bool, Message )                      \
	do                                                                \
		if( not( Bool ) ) {                                             \
			std::stringstream ss{ };                                      \
			ss << "Invalid result. Expecting '" #Bool << "' to be true\n" \
			   << "Message: " #Message << "File: " << __FILE__            \
			   << "\nLine: " << __LINE__;                                 \
			throw std::logic_error( ss.str( ) );                          \
		}                                                               \
	while( false )
#else
#define daw_expecting_message( Bool, Message )                             \
	do                                                                       \
		if( not( Bool ) ) {                                                    \
			std::stringstream ss{ };                                             \
			std::cerr << "Invalid result. Expecting '" #Bool << "' to be true\n" \
			          << "Message: " #Message << "File: " << __FILE__            \
			          << "\nLine: " << __LINE__ << std::endl;                    \
			std::abort( );                                                       \
		}                                                                      \
	while( false )
#endif

using namespace daw::sv2::string_view_literals;

namespace daw {
	enum class tmp_e { a, b, c };

	constexpr bool is_equal_nc( daw::sv2::string_view lhs,
	                            daw::sv2::string_view rhs ) noexcept {
		if( lhs.size( ) != rhs.size( ) ) {
			return false;
		}
		bool result = true;
		for( size_t n = 0; n < lhs.size( ); ++n ) {
			result &= ( lhs[n] | ' ' ) == ( rhs[n] | ' ' );
		}
		return result;
	}

	constexpr tmp_e tmp_e_from_str( daw::sv2::string_view str ) {
		if( is_equal_nc( str, "a" ) ) {
			return tmp_e::a;
		}
		if( is_equal_nc( str, "b" ) ) {
			return tmp_e::b;
		}
		if( is_equal_nc( str, "c" ) ) {
			return tmp_e::c;
		}
#if defined( DAW_USE_EXCEPTIONS )
		throw std::runtime_error( "unknown http request method" );
#else
		std::cerr << "Unknown http request method\n";
		std::abort( );
#endif
	}

	constexpr daw::sv2::string_view do_something( daw::sv2::string_view str,
	                                              tmp_e &result ) {
		str = str.substr( 0, str.find_first_of( ' ' ) );
		result = tmp_e_from_str( str );

		return str;
	}

	constexpr void daw_string_view_constexpr_001( ) {
		daw::sv2::string_view a = "A test";
		tmp_e result = tmp_e::b;
		auto str = do_something( a, result );
		::Unused( str );
		daw_expecting( result == tmp_e::a, true );
	}

	void daw_string_view_find_last_of_001( ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		std::string_view const b = "abcdefghijklm";
		auto const pos = a.find_last_of( "ij" );
		auto const pos2 = b.find_last_of( "ij" );
		daw_expecting( pos, pos2 );

		auto const es = a.find_last_of( "lm" );
		auto const es2 = b.find_last_of( "lm" );
		daw_expecting( es, es2 );
	}

	void daw_string_view_find_last_of_002( ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( "", 0 );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_of_003( ) {
		auto a = daw::sv2::string_view( );
		auto const pos = a.find_last_of( "a" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_of_004( ) {
		auto a = daw::sv2::string_view( "this is a test" );
		auto const pos = a.find_last_of( "a", 5 );
		daw_expecting( 8U, pos );
	}

	constexpr void
	daw_string_view_find_last_of_005( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	constexpr void
	daw_string_view_find_last_of_006( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle, 1000U );
		daw_expecting( 0U, pos );
	}

	constexpr void
	daw_string_view_find_last_of_007( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle, a.size( ) - 1 );
		daw_expecting( 0U, pos );
	}

	void daw_string_view_find_first_of_if_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) {
			return c == 'c';
		} );
		daw_expecting( 2U, pos );
	}

	void daw_string_view_find_first_of_if_002( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if(
		  []( auto c ) {
			  return c == 'c';
		  },
		  1000 );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_if_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) {
			return c == 'x';
		} );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_if_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) {
			return c < 'c';
		} );
		daw_expecting( 2U, pos );
	}

	void daw_string_view_find_first_not_of_if_002( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.find_first_not_of_if( []( auto c ) {
			return c < 'c';
		} );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_if_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) {
			return c < 'x';
		} );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def" );
		daw_expecting( 3U, pos );
	}

	void daw_string_view_find_first_of_002( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def", 100 );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_004( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "x" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_001( ) {
		daw::sv2::string_view const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc" );
		daw_expecting( 5U, pos );
	}

	void daw_string_view_find_first_not_of_002( ) {
		daw::sv2::string_view const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc", 1000 );
		daw_expecting( daw::sv2::string_view::npos, pos );
		pos = a.find_first_not_of( "" );
		daw_expecting( 0U, pos );
		pos = a.find_first_not_of( "abc", 4U );
		daw_expecting( 5U, pos );
		pos = a.find_first_not_of( a );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_not_of_001( ) {
		std::string_view const str = "abcabfghijklm";
		daw::sv2::string_view const sv = "abcabfghijklm";
		auto pos = str.find_last_not_of( "abc" );
		auto pos_sv = sv.find_last_not_of( "abc" );
		daw_expecting( pos, pos_sv );
	}

	void daw_string_view_find_last_not_of_002( ) {
		std::string_view const str = "abcabfghijklmabc";
		daw::sv2::string_view const sv = str;

		for( std::size_t n = 0; n < sv.size( ); ++n ) {
			auto pos = str.find_last_not_of( "abc", n );
			auto pos_sv = sv.find_last_not_of( "abc", n );
			daw_expecting( pos, pos_sv );
		}
	}

	void daw_string_view_find_last_not_of_003( ) {
		std::string_view const str = "abcabfghijklmabc";
		daw::sv2::string_view const sv = str;
		daw::sv2::string_view const needle = "abc";

		for( std::size_t n = 0; n < sv.size( ); ++n ) {
			auto pos = str.find_last_not_of( "abc", n );
			auto pos_sv = sv.find_last_not_of( needle, n );
			daw_expecting( pos, pos_sv );
		}
	}

	void daw_string_view_find_last_not_of_004( ) {
		daw::sv2::string_view const sv{ };
		auto pos = sv.find_last_not_of( "a" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_not_of_005( ) {
		daw::sv2::string_view const sv{ "dfsdfsdfsd" };
		auto pos = sv.find_last_not_of( "" );
		daw_expecting( 9U, pos );
	}

	void daw_string_view_find_last_not_of_if_001( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if( []( char c ) {
			return std::isspace( c ) != 0;
		} );
		daw_expecting( 13U, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_002( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if(
		  []( char c ) {
			  return std::isspace( c ) != 0;
		  },
		  6 );
		daw_expecting( 5U, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_003( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv = sv.find_last_not_of_if( []( char c ) {
			return std::isspace( c ) != 0;
		} );
		daw_expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_004( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if( []( char ) {
			return false;
		} );
		daw_expecting( sv.size( ) - 1, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_005( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if( []( char ) {
			return true;
		} );
		daw_expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_string_view_search_001( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search( "aaa" );
		daw_expecting( 5U, pos );
	}

	void daw_string_view_search_002( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.search( "aaa" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_003( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search( "xdf" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_001( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa" );
		daw_expecting( 5U, pos );
	}

	void daw_string_view_search_last_002( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa", 3 );
		daw_expecting( 5U, pos );
	}

	void daw_string_view_search_last_003( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_004( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.search_last( "a" );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_005( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa", 100 );
		daw_expecting( daw::sv2::string_view::npos, pos );
	}

	void tc001( ) {
		daw::sv2::string_view view;
		puts( "Constructs an empty string" );

		{ daw_expecting( view.empty( ), true ); }
		puts( "Is 0 bytes in size" );

		{ daw_expecting( view.size( ), 0 ); }

		puts( "Points to null" );

		{ daw_expecting( view.data( ), nullptr ); }
	}

	//----------------------------------------------------------------------------

	void tc002( ) {
		std::string str = "Hello world";
		daw::sv2::string_view view = str;

		puts( "Constructs a non-empty string" );
		{ daw_expecting( view.empty( ), false ); }

		puts( "Has non-zero size" );
		{ daw_expecting( view.size( ) != 0, true ); }

		puts( "Points to original string" );
		{ daw_expecting( view.data( ), str.data( ) ); }
	}
	//----------------------------------------------------------------------------

	void tc003( ) {
		auto empty_str = "";
		auto non_empty_str = "Hello World";

		puts( "Is empty with empty string" );
		{
			daw::sv2::string_view view = empty_str;

			daw_expecting( view.empty( ), true );
		}

		puts( "Is non-empty with non-empty string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw_expecting( view.empty( ), false );
		}

		puts( "Is size 0 with empty string" );
		{
			daw::sv2::string_view view = empty_str;

			daw_expecting( view.size( ), 0 );
		}

		puts( "Is not size 0 with non-empty string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw_expecting( view.size( ) != 0, true );
		}

		puts( "Points to original string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw_expecting( view.data( ), non_empty_str );
		}
	}

	//----------------------------------------------------------------------------

	void tc004( ) {
		daw::sv2::string_view empty = "";
		daw::sv2::string_view view = "Hello world";

		puts( "Is empty with empty string" );
		{ daw_expecting( empty.empty( ), true ); }

		puts( "Is non-empty with non-empty string" );
		{ daw_expecting( view.empty( ), false ); }

		puts( "Is size 0 with empty string" );
		{ daw_expecting( empty.size( ), 0 ); }

		puts( "Is not size 0 with non-empty string" );
		{ daw_expecting( view.size( ) != 0, true ); }
	}

	//----------------------------------------------------------------------------
	// Capacity
	//----------------------------------------------------------------------------

	void tc004capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns non-zero for non-empty string" );
		{
			daw_expecting( view.size( ), ( std::char_traits<char>::length( str ) ) );
		}

		puts( "Returns 0 for empty string" );
		{
			view = "";

			daw_expecting( view.size( ), 0 );
		}
	}

	//----------------------------------------------------------------------------

	void tc005capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns non-zero for non-empty string" );
		{ daw_expecting( view.length( ), std::char_traits<char>::length( str ) ); }

		puts( "Returns 0 for empty string" );
		{
			view = "";

			daw_expecting( view.length( ), 0 );
		}
	}

	//----------------------------------------------------------------------------

	void tc006capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns false on non-empty string" );
		{ daw_expecting( view.empty( ), false ); }

		puts( "Returns true on empty string" );
		{
			view = "";

			daw_expecting( view.empty( ), true );
		}
	}

	//----------------------------------------------------------------------------
	// Element Access
	//----------------------------------------------------------------------------

	void tc007accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Points to original data source" );
		{ daw_expecting( view.data( ), str ); }
	}

	//----------------------------------------------------------------------------

	void tc008accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Points to original data source" );
		{ daw_expecting( view.data( ), str ); }
	}

	//----------------------------------------------------------------------------

	void tc009accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns reference to entry at position" );
		{ daw_expecting( &view[0], str ); }
	}

	//----------------------------------------------------------------------------

	void tc010accessor( ) {
#if defined( DAW_USE_EXCEPTIONS )
		constexpr char const str[] = "Hello World";
		daw::sv2::string_view view = +str;

		puts( "Returns reference to entry at position" );
		{ daw_expecting( &view.at( 0 ), str ); }

		puts( "Throws when out of range" );
		bool good = false;
		try {
			(void)view.at( 11 );
		} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
		}
		if( not good ) {
			puts( "Expected out of range exception" );
			std::abort( );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc011accessor( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Returns reference to first character" );
		{ daw_expecting( view.front( ), 'H' ); }
	}

	//----------------------------------------------------------------------------

	void tc012accessor( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Returns reference to last character" );
		{ daw_expecting( view.back( ), 'd' ); }
	}

	//----------------------------------------------------------------------------
	// Modifiers
	//----------------------------------------------------------------------------

	void tc013modifier( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Removes first n characters" );
		{
			view.remove_prefix( 6 );
			auto result = view == "World";
			daw_expecting( result, true );
		}
	}

	//----------------------------------------------------------------------------

	void tc014modifier( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Removes last n characters" );
		{
			view.remove_suffix( 6 );

			daw_expecting( view, "Hello" );
			daw_expecting( view, "Hello" );
		}
	}

	//----------------------------------------------------------------------------

	void tc015modifier( ) {
		const char *str1 = "Hello World";
		const char *str2 = "Goodbye World";
		daw::sv2::string_view view1 = str1;
		daw::sv2::string_view view2 = str2;

		puts( "Swaps entries" );
		{
			std::swap( view1, view2 );

			daw_expecting( ( ( view1.data( ) == str2 ) && ( view2.data( ) == str1 ) ),
			               true );
		}
	}

	//----------------------------------------------------------------------------
	// String Operations
	//----------------------------------------------------------------------------
	void tc016conversion( ) {
		daw::sv2::string_view view = "Hello World";

		std::string string = static_cast<std::string>( view );

		puts( "Copies view to new location in std::string" );
		{ daw_expecting( view.data( ) != string.data( ), true ); }

		puts( "Copied string contains same contents as view" );
		{ daw_expecting( string, "Hello World" ); }
	}
	//----------------------------------------------------------------------------

	void tc017conversion( ) {
		daw::sv2::string_view view = "Hello World";

		std::string string = static_cast<std::string>( view );

		puts( "Copies view to new location in std::string" );
		{ daw_expecting( view.data( ) != string.data( ), true ); }

		puts( "Copied string contains same contents as view" );
		{ daw_expecting( string, "Hello World" ); }
	}

	//----------------------------------------------------------------------------
	// Operations
	//----------------------------------------------------------------------------

	void tc018operation( ) {
#if defined( DAW_USE_EXCEPTIONS )
		daw::sv2::string_view const view = "Hello World";

		puts( "Throws std::out_of_range if pos >= view.size()" );
		{
			char result[12];

			bool good = false;
			try {
				view.copy( result, 12, 12 );
			} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
			}
			if( not good ) {
				puts( "Expected out_of_range_exception" );
				std::abort( );
			}
		}

		puts( "Copies entire string" );
		{
			char result[11];
			view.copy( result, 11 );

			daw_expecting( std::strncmp( result, "Hello World", 11 ), 0 );
		}

		puts( "Copies remaining characters if count > size" );
		{
			char result[11];
			view.copy( result, 20 );

			daw_expecting( std::strncmp( result, "Hello World", 11 ), 0 );
		}

		puts( "Copies part of the string" );
		{
			char result[11];
			view.copy( result, 5 );

			daw_expecting( std::strncmp( result, "Hello", 5 ), 0 );
		}

		puts( "Copies part of the string, offset from the beginning" );
		{
			char result[11];
			view.copy( result, 10, 6 );

			daw_expecting( std::strncmp( result, "World", 5 ), 0 );
		}

		puts( "Returns number of characters copied" );
		{
			char result[11];
			daw_expecting( view.copy( result, 20 ), 11 );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc019operation( ) {
#if defined( DAW_USE_EXCEPTIONS )
		daw::sv2::string_view view = "Hello World";

		puts( "Returns the full string when given no args" );
		{
			auto substr = view.substr( );
			daw_expecting( substr, "Hello World" );
		}

		puts( "Returns last part of string" );
		{
			auto substr = view.substr( 6 );
			daw_expecting( substr, "World" );
		}

		puts( "Substring returns at most count characters" );
		{
			auto substr = view.substr( 6, 1 );
			daw_expecting( substr, "W" );
			daw_expecting( substr, "W" );
		}

		puts( "Returns up to end of string if length > size" );
		{
			auto substr = view.substr( 6, 10 );
			daw_expecting( substr, "World" );
		}

		puts( "Throws std::out_of_range if pos > size" );

		bool good = false;
		try {
			(void)view.substr( 15 );
		} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
		}
		if( not good ) {
			puts( "Expected out_of_range_exception" );
			std::abort( );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc020comparison( ) {
		puts( "Returns 0 for identical views" );
		{
			daw::sv2::string_view view = "Hello World";

			daw_expecting( view.compare( "Hello World" ), 0 );
		}

		puts( "Returns nonzero for different views" );
		{
			daw::sv2::string_view view = "Hello World";

			daw_expecting( view.compare( "Goodbye World" ) != 0, true );
		}

		puts( "Returns > 0 for substring beginning with same string" );
		{
			daw::sv2::string_view view = "Hello World";

			daw_expecting( view.compare( "Hello" ) > 0, true );
		}

		puts( "Returns < 0 for superstring beginning with self" );
		{
			daw::sv2::string_view view = "Hello";

			daw_expecting( view.compare( "Hello World" ) < 0, true );
		}

		puts(
		  "Returns < 0 for same-sized string compared to character greater than "
		  "char" );
		{
			daw::sv2::string_view view = "1234567";

			daw_expecting( view.compare( "1234667" ) < 0, true );
		}

		puts(
		  "Returns > 0 for same-sized string compared to character less than "
		  "char" );
		{
			daw::sv2::string_view view = "1234567";

			daw_expecting( view.compare( "1234467" ) > 0, true );
		}
	}

	//----------------------------------------------------------------------------

	void tc021comparison( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Is equal" );
		{
			puts( "Returns true for equal string views" );
			{
				daw::sv2::string_view view2 = "Hello World";

				daw_expecting( view, view2 );
			}

			puts( "Returns true for equal strings with left char array" );
			{ daw_expecting( "Hello World", view ); }

			puts( "Returns true for equal strings with right char array" );
			{ daw_expecting( view, "Hello World" ); }

			puts( "Returns true for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw_expecting( str, view );
			}

			puts( "Returns true for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw_expecting( view, str );
			}

			puts( "Returns true for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw_expecting( str, view );
			}

			puts( "Returns true for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw_expecting( view, str );
			}
		}

		puts( "Is not equal" );
		{
			puts( "Returns false for non-equal string views" );
			{
				daw::sv2::string_view view2 = "Goodbye World";

				daw_expecting( false, ( view == view2 ) );
			}

			puts( "Returns false for non-equal strings with left char array" );
			{ daw_expecting( false, ( ( "Goodbye World" == view ) ) ); }

			puts( "Returns false for non-equal strings with right char array" );
			{ daw_expecting( false, ( ( view == "Goodbye World" ) ) ); }

			puts( "Returns false for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw_expecting( false, ( str == view ) );
			}

			puts( "Returns false for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw_expecting( false, ( view == str ) );
			}

			puts( "Returns false for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw_expecting( false, ( str == view ) );
			}

			puts( "Returns false for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw_expecting( false, ( view == str ) );
			}

			puts( "Two unequal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "2";
				daw::sv2::string_view sv1{ str1 };
				daw::sv2::string_view sv2{ str2 };
				daw_expecting( false, ( sv1 == sv2 ) );
			}
			puts( "Two equal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "1";
				daw::sv2::string_view sv1{ str1 };
				daw::sv2::string_view sv2{ str2 };
				daw_expecting( sv1, sv2 );
			}
		}
	}

	//----------------------------------------------------------------------------

	void tc022comparison( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Is equal" );
		{
			puts( "Returns false for equal string views" );
			{
				daw::sv2::string_view view2 = "Hello World";

				daw_expecting( false, ( view != view2 ) );
			}

			puts( "Returns false for equal strings with left char array" );
			{ daw_expecting( false, ( "Hello World" != view ) ); }

			puts( "Returns false for equal strings with right char array" );
			{ daw_expecting( false, ( view != "Hello World" ) ); }

			puts( "Returns false for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw_expecting( false, ( str != view ) );
			}

			puts( "Returns false for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw_expecting( false, ( view != str ) );
			}

			puts( "Returns false for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw_expecting( false, ( str != view ) );
			}

			puts( "Returns false for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw_expecting( false, ( view != str ) );
			}
		}

		puts( "Is not equal" );
		{
			puts( "Returns true for non-equal string views" );
			{
				daw::sv2::string_view view2 = "Goodbye World";

				daw_expecting( view != view2, true );
			}

			puts( "Returns true for non-equal strings with left char array" );
			{ daw_expecting( "Goodbye World" != view, true ); }

			puts( "Returns true for non-equal strings with right char array" );
			{ daw_expecting( view != "Goodbye World", true ); }

			puts( "Returns true for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw_expecting( str != view, true );
			}

			puts( "Returns true for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw_expecting( view != str, true );
			}

			puts( "Returns true for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw_expecting( str != view, true );
			}

			puts( "Returns true for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw_expecting( view != str, true );
			}
		}
	}

	void daw_can_be_string_view_starts_with_001( ) {
		daw_expecting(
		  daw::sv2::string_view{ "This is a test" }.starts_with( "This" ), true );
	}

	void daw_can_be_string_view_starts_with_002( ) {
		daw_expecting( daw::sv2::string_view{ "This is a test" }.starts_with(
		                 daw::sv2::string_view{ "This" } ),
		               true );
	}

	void daw_can_be_string_view_starts_with_003( ) {
		daw_expecting( daw::sv2::string_view{ "This is a test" }.starts_with( 'T' ),
		               true );
	}

	void daw_can_be_string_view_starts_with_004( ) {
		daw_expecting(
		  not daw::sv2::string_view{ "This is a test" }.starts_with( "ahis" ),
		  true );
	}

	void daw_can_be_string_view_starts_with_005( ) {
		daw_expecting( not daw::sv2::string_view{ "This is a test" }.starts_with(
		                 daw::sv2::string_view{ "ahis" } ),
		               true );
	}

	void daw_can_be_string_view_starts_with_006( ) {
		daw_expecting(
		  not daw::sv2::string_view{ "This is a test" }.starts_with( 'a' ), true );
	}

	void daw_can_be_string_view_starts_with_007( ) {
		daw_expecting( not daw::sv2::string_view{ }.starts_with( 'a' ), true );
	}

	void daw_can_be_string_view_starts_with_008( ) {
		daw_expecting( not daw::sv2::string_view{ }.starts_with( " " ), true );
	}

	void daw_can_be_string_view_starts_with_009( ) {
		daw_expecting( not daw::sv2::string_view{ " " }.starts_with( "    " ),
		               true );
	}

	void daw_can_be_string_view_ends_with_001( ) {
		daw_expecting(
		  daw::sv2::string_view{ "This is a test" }.ends_with( "test" ), true );
	}

	void daw_can_be_string_view_ends_with_002( ) {
		daw_expecting( daw::sv2::string_view{ "This is a test" }.ends_with(
		                 daw::sv2::string_view{ "test" } ),
		               true );
	}

	void daw_can_be_string_view_ends_with_003( ) {
		daw_expecting( daw::sv2::string_view{ "This is a test" }.ends_with( 't' ),
		               true );
	}

	void daw_can_be_string_view_ends_with_004( ) {
		daw_expecting(
		  not daw::sv2::string_view{ "This is a test" }.ends_with( "aest" ), true );
	}

	void daw_can_be_string_view_ends_with_005( ) {
		daw_expecting( not daw::sv2::string_view{ "This is a test" }.ends_with(
		                 daw::sv2::string_view{ "aest" } ),
		               true );
	}

	void daw_can_be_string_view_ends_with_006( ) {
		daw_expecting(
		  not daw::sv2::string_view{ "This is a test" }.ends_with( 'a' ), true );
	}

	void daw_can_be_string_view_ends_with_007( ) {
		daw_expecting( not daw::sv2::string_view{ "a" }.ends_with( "hello" ),
		               true );
	}

	void daw_can_be_string_view_ends_with_008( ) {
		daw_expecting( not daw::sv2::string_view{ }.ends_with( ' ' ), true );
	}

	void daw_can_be_string_view_ends_with_009( ) {
		daw_expecting( not daw::sv2::string_view{ }.ends_with( " " ), true );
	}

	void daw_can_be_string_view_ends_with_010( ) {
		static constexpr char const *needle = "a";
		daw_expecting(
		  not daw::sv2::string_view{ "This is a test" }.ends_with( needle ), true );
	}

	void daw_pop_front_until_sv_test_001( ) {
		daw::sv2::string_view sv = "This is a test";
		daw_expecting( "This"_sv, sv.pop_front_until( " " ) );
		daw_expecting( "is"_sv, sv.pop_front_until( " " ) );
		daw_expecting( "a"_sv, sv.pop_front_until( " " ) );
		daw_expecting( "test"_sv, sv.pop_front_until( " " ) );
		daw_expecting( sv.empty( ), true );
	}

	void daw_pop_front_test_001( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_front( );
		daw_expecting( result, 'T' );
	}

	void daw_pop_front_count_test_001( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_front( 4 );
		daw_expecting( result, "This" );
	}

	void daw_pop_back_count_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.pop_back( 4 );
		daw_expecting( result, "test" );
		daw_expecting( sv, "This is a " );
	}

	void daw_pop_back_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw_expecting( "test"_sv, sv.pop_back_until( " " ) );
		daw_expecting( "a"_sv, sv.pop_back_until( " " ) );
		daw_expecting( "is"_sv, sv.pop_back_until( " " ) );
		daw_expecting( "This"_sv, sv.pop_back_until( " " ) );
		daw_expecting( sv.empty( ), true );
	}

	void daw_pop_back_until_sv_test_002( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_back_until( []( char c ) {
			return c == 'x';
		} );
		daw_expecting_message( sv.empty( ), "Expected empty result" );
		daw_expecting_message( result.end( ) == sv.end( ), "Expected same end( )" );
	}

	void daw_pop_front_pred_test_001( ) {
		std::string_view str = "This is1a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto lhs = sv.pop_front_until( []( auto c ) {
			return std::isdigit( c );
		} );
		daw_expecting( lhs, "This is" );
		daw_expecting( sv, "a test" );
	}

	void daw_pop_back_pred_test_001( ) {
		daw::sv2::string_view sv = "This is1a test";
		auto result = sv.pop_back_until( []( auto c ) {
			return std::isdigit( c );
		} );
		daw_expecting( "This is", sv );
		daw_expecting( "a test", result );
	}

	void daw_try_pop_back_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw_expecting( sv.try_pop_back_until( " " ), "test" );
		daw_expecting( sv.try_pop_back_until( " " ), "a" );
		daw_expecting( sv.try_pop_back_until( " " ), "is" );
		daw_expecting( sv.try_pop_back_until( " " ), "" );
		daw_expecting( sv, "This" );
	}

	void daw_try_pop_back_until_sv_test_002( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.try_pop_back_until( "blah" );
		daw_expecting( result.empty( ), true );
		daw_expecting( sv, str );
	}

	void daw_try_pop_front_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw_expecting( sv.try_pop_front_until( " " ), "This" );
		daw_expecting( sv.try_pop_front_until( " " ), "is" );
		daw_expecting( sv.try_pop_front_until( " " ), "a" );
		daw_expecting( sv.try_pop_front_until( " " ) != "test", true );
		daw_expecting( sv, "test" );
	}

	void daw_try_pop_front_until_sv_test_002( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.try_pop_front_until( "blah" );
		daw_expecting( result.empty( ), true );
		daw_expecting( sv, str );
		daw::sv2::string_view s{ };
		s.remove_prefix( );
	}

	constexpr bool ensure_same_at_ct( ) {
		constexpr daw::sv2::string_view sv_cx = "a";
		static_assert( sv_cx.size( ) == 1 );
		static_assert( not sv_cx.empty( ) );
		static_assert( *( sv_cx.data( ) +
		                  static_cast<std::ptrdiff_t>( sv_cx.size( ) ) ) == '\0' );
		static_assert( sv_cx.end( ) - sv_cx.begin( ) == 1 );
		daw::sv2::string_view sv_run = "a";
		if( sv_run != sv_cx ) {
			std::cerr << "Runtime/Constexpr not the same\n";
			std::cerr << "Should have size 1 and not be empty\n";
			std::cerr << "size( ) = " << sv_cx.size( );
			std::cerr << " empty( ) = " << sv_cx.empty( ) << '\n';
			return false;
		}
		return true;
	}

	void daw_diff_assignment_001( ) {
#if not defined( _MSC_VER ) or defined( __clang__ )
		daw::sv2::basic_string_view<char, daw::sv2::string_view_bounds_type::size>
		  a = "This is a test";
		daw::sv2::basic_string_view<char,
		                            daw::sv2::string_view_bounds_type::pointer>
		  b = "Hello";
		daw_expecting_message( a != b, "Expected equal" );
		static_assert( not std::is_same_v<decltype( a ), decltype( b )> );
		// Should have different types
		b = a;
		daw_expecting_message( a == b, "Expected equal" );
#endif
	}

	void daw_literal_test_001( ) {
		constexpr auto sv_char = "Hello"_sv;
		static_assert(
		  std::is_same_v<char, typename decltype( sv_char )::value_type> );
		constexpr auto sv_wchar_t = L"Hello"_sv;
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
#if defined( __cpp_char8_t )
		constexpr auto sv_char8_t = u8"Hello"_sv;
		static_assert(
		  std::is_same_v<char8_t, typename decltype( sv_char8_t )::value_type> );
#endif
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
		constexpr auto sv_char16_t = u"Hello"_sv;
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
		static_assert(
		  std::is_same_v<char16_t, typename decltype( sv_char16_t )::value_type> );
		constexpr auto sv_char32_t = U"Hello"_sv;
		static_assert(
		  std::is_same_v<char32_t, typename decltype( sv_char32_t )::value_type> );
	}

	void daw_stdhash_test_001( ) {
		std::hash<daw::sv2::string_view> h{ };
		daw::sv2::string_view message = "Hello World!";
		auto hash = h( message );
		if constexpr( daw::impl::is_64bit_v ) {
			daw_expecting( std::uint64_t{ 0x8C0E'C8D1'FB9E'6E32ULL }, hash );
		} else {
			daw_expecting( std::uint32_t{ 0xB1EA'4872ULL }, hash );
		}
	}

	void daw_rfind_test_001( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.rfind( "is" );
		daw_expecting( 5U, pos_sv );
	}

	void daw_rfind_test_002( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv = sv.rfind( "is" );
		daw_expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_rfind_test_003( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.rfind( "" );
		daw_expecting( 16U, pos_sv );
	}

	void daw_find_test_001( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.find( "is" );
		daw_expecting( 2U, pos_sv );
	}

	void daw_find_test_002( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv = sv.find( "is" );
		daw_expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_find_test_003( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.find( "" );
		daw_expecting( 0U, pos_sv );
	}
	namespace url_test {
		struct authority_t {
			daw::sv2::string_view host;
			daw::sv2::string_view port;

			constexpr authority_t( daw::sv2::string_view sv ) noexcept
			  : host( sv.pop_front_until( ':' ) )
			  , port( sv ) {}

			constexpr authority_t( daw::sv2::string_view h,
			                       daw::sv2::string_view p ) noexcept
			  : host( h )
			  , port( p ) {}

			constexpr bool operator==( authority_t const &rhs ) const {
				return host == rhs.host and ( ( port == rhs.port ) or
				                              ( port == "80" and rhs.port.empty( ) ) or
				                              ( port.empty( ) and rhs.port == "80" ) );
			}

			constexpr bool operator!=( authority_t const &rhs ) const {
				return not operator==( rhs );
			}
		};

		struct query_t {
			std::map<daw::sv2::string_view, daw::sv2::string_view> items{ };

			query_t( ) = default;

			explicit query_t( daw::sv2::string_view sv ) {
				while( not sv.empty( ) ) {
					auto p = sv.pop_front_until( '&' );
					items.insert( std::pair{ p.pop_front_until( '=' ), p } );
				}
			}

			inline explicit query_t(
			  std::initializer_list<
			    std::pair<daw::sv2::string_view, daw::sv2::string_view>>
			    qparts )
			  : items( qparts.begin( ), qparts.end( ) ) {}

			inline bool operator==( query_t const &rhs ) const {
				return items == rhs.items;
			}

			inline bool operator!=( query_t const &rhs ) const {
				return not operator==( rhs );
			}
		};

		struct uri_parts {
			daw::sv2::string_view scheme;
			authority_t authority;
			daw::sv2::string_view path;
			query_t query;
			daw::sv2::string_view fragment;
		};
		// scheme://authority:port/path?query#fragment
		uri_parts parse_url( daw::sv2::string_view uri_string ) {
			using namespace daw::sv2;
			auto scheme = uri_string.pop_front_until( "://" );
			auto authority =
			  uri_string.pop_front_until( any_of<'/', '?', '#'>, nodiscard );
			auto path = uri_string.pop_front_until( any_of<'?', '#'> );
			auto query = query_t( uri_string.pop_front_until( '#' ) );

			return { scheme, authority, path, query, uri_string };
		}
	} // namespace url_test

	void daw_test_any_char_001( ) {
		using namespace daw::sv2::string_view_literals;
		auto [scheme, authority, path, query, fragment] =
		  url_test::parse_url( "https://www.google.com?q=hello#line1" );
		constexpr auto expected_authority =
		  url_test::authority_t( "www.google.com", "" );
		auto const expected_query =
		  url_test::query_t( { std::pair{ "q"_sv, "hello"_sv } } );
		daw_expecting( scheme, "https" );
		daw_expecting( authority, expected_authority );
		daw_expecting( path, "" );
		daw_expecting( query, expected_query );
		daw_expecting( fragment, "line1" );
	}

	void daw_remove_prefix_num_test_001( ) {
		daw::sv2::string_view sv = "0123456789";
		sv.remove_prefix( 4 );
		daw_expecting( sv.size( ), 6U );
		daw_expecting( sv.front( ), '4' );
	}

	void daw_remove_prefix_num_test_002( ) {
		daw::sv2::string_view sv = "0123456789";
		sv.remove_prefix( 1000 );
		daw_expecting( sv.empty( ), true );
	}

	void daw_remove_prefix_num_test_003( ) {
		daw::sv2::string_view sv = "0123456789";
		auto const sv_orig = sv;
		sv.remove_prefix( 0 );
		daw_expecting( sv, sv_orig );
	}

	void daw_remove_prefix_until_test_001( ) {
		daw::sv2::string_view sv = "0123456789";
		sv.remove_prefix_until( '5' );
		daw_expecting( sv.size( ), 4U );
		daw_expecting( sv.front( ), '6' );
	}

	void daw_remove_prefix_until_test_002( ) {
		daw::sv2::string_view sv = "0123456789";
		sv.remove_prefix_until( '5', daw::nodiscard );
		daw_expecting( sv.size( ), 5U );
		daw_expecting( sv.front( ), '5' );
	}

	void daw_remove_prefix_until_test_003( ) {
		daw::sv2::string_view sv = "This is a test";
		sv.remove_prefix_until( daw::any_of<' ', '\n', '\t'> );
		daw_expecting( sv.size( ), 9U );
		daw_expecting( sv, "is a test" );
	}

	void daw_remove_prefix_until_test_004( ) {
		daw::sv2::string_view sv = "'0123456789\n#this is a test\n";
		sv.remove_prefix_until( '\n' );
		daw_expecting( sv.size( ), 16U );
		daw_expecting( sv.front( ), '#' );
	}

	void daw_remove_suffix_while_test_001( ) {
		daw::sv2::string_view sv = "test\n";
		sv.remove_suffix_while( '\n' );
		auto const sz = sv.size( );
		daw_expecting( sz, 4 );
	}

	void daw_remove_suffix_while_test_002( ) {
		daw::sv2::string_view sv = "test\n";
		sv.remove_suffix_while( 'a' );
		auto const sz = sv.size( );
		daw_expecting( sz, 5 );
	}

	struct FooSV {
		using iterator = char const *;
		using size_type = std::size_t;
		iterator p;
		size_type s;

		constexpr iterator data( ) const {
			return p;
		}

		constexpr size_type size( ) const {
			return s;
		}
	};

	void daw_arbutrary_string_view_list_001( ) {
		daw::sv2::string_view sv = "Hello world";
		auto foo = FooSV{ std::data( sv ), std::size( sv ) };
		daw::sv2::string_view svfoo = foo;
		daw_expecting( svfoo.size( ), sv.size( ) );
		daw_expecting( svfoo, sv );
	}

	void daw_trim_prefix_test_001( ) {
		daw::sv2::string_view sv = "  Hello World";
		sv.trim_prefix( );
		daw_expecting( sv, "Hello World" );
	}

	void daw_trim_prefix_test_002( ) {
		daw::sv2::string_view sv = "";
		sv.trim_prefix( );
		daw_expecting( sv, "" );
	}

	void daw_trim_suffix_test_001( ) {
		daw::sv2::string_view sv = "Hello World  ";
		sv.trim_suffix( );
		daw_expecting( sv, "Hello World" );
	}

	void daw_trim_suffix_test_002( ) {
		daw::sv2::string_view sv = "";
		sv.trim_suffix( );
		daw_expecting( sv, "" );
	}

	void daw_construct_from_string_001( ) {
		std::string foo = "Hello World";
		auto bar = daw::sv2::string_view( foo );
		(void)bar;
	}

	void implicitly_ctor_test( daw::sv2::string_view ) {}

	void daw_construct_from_string_002( ) {
		std::string foo = "Hello World";
		implicitly_ctor_test( foo );
	}

	void daw_construct_from_string_view_001( ) {
		std::string_view foo = "Hello World";
		auto bar = daw::sv2::string_view( foo );
		(void)bar;
	}

	void daw_construct_from_string_view_002( ) {
		std::string_view foo = "Hello World";
		implicitly_ctor_test( foo );
	}

	void daw_construct_from_string_literal_001( ) {
		auto bar = daw::sv2::string_view( "Hello World" );
		(void)bar;
	}

	void daw_construct_from_string_literal_002( ) {
		implicitly_ctor_test( "Hello World" );
	}

	void daw_convert_to_std_string( ) {
		daw::sv2::string_view sv = "Hello World";
		auto str = static_cast<std::string>( sv );
		(void)str;
	}

	void daw_convert_to_std_string_view( ) {
		daw::sv2::string_view sv = "Hello World";
		auto stdsv = static_cast<std::string_view>( sv );
		(void)stdsv;
	}

	void daw_from_charptr( ) {
		char const *ptr = "Hello";
		auto sv = daw::basic_string_view( ptr );
		(void)sv;
	}

#if defined( __cpp_char8_t )
	void daw_from_char8ptr( ) {
		char8_t const *ptr = u8"Hello";
		auto sv = daw::basic_string_view( ptr );
		(void)sv;
	}
#endif

	void daw_trim( ) {
		daw::sv2::string_view sv = " Hello ";
		(void)sv.trim( );
		daw_expecting( sv, "Hello" );
	}

	void daw_trim_copy( ) {
		daw::sv2::string_view sv = " Hello ";
		auto sv2 = sv.trim_copy( );
		daw_expecting( sv != sv2, true );
		daw_expecting( sv2, "Hello" );
	}

	namespace daw_array_ctad_impl {
		template<typename S>
		constexpr auto foo( S &&s ) {
			return daw::sv2::basic_string_view( s );
		}
	} // namespace daw_array_ctad_impl

	void daw_array_ctad( ) {
		auto f = daw_array_ctad_impl::foo( "Test" );
		daw_expecting( f, "Test" );
	}

	void daw_find_first_match( ) {
		daw::sv2::string_view sv = R"(Hello 😍 World)";
		auto pos = sv.find_first_match( { "😍", "Wor" } );
		daw_expecting( pos, 6 );
		pos = sv.find_first_match( { "Wor" } );
		daw_expecting( pos, 11 );
	}

#if defined( DAW_HAS_CPP20_3WAY_COMPARE )
	void daw_3way_compare_test_001( ) {
		daw::sv2::string_view lhs = "Hello";
		daw::sv2::string_view rhs = "Bye";
		// clang-format off
		auto cmp = lhs <=> rhs;
		// clang-format on
		static_assert( std::is_same_v<std::strong_ordering, decltype( cmp )> );
		daw_expecting( cmp, std::strong_ordering::greater );
	}
#endif
} // namespace daw

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	/*
	 * Ensure that daw::sv2::string_view s = { "a", "b" }; doesn't work
	 */
	static_assert(
	  not std::is_constructible_v<daw::sv2::string_view, char const( & )[4],
	                              char const( & )[4]> );
	static_assert( std::is_constructible_v<daw::sv2::string_view, char const *,
	                                       char const *> );
	static_assert(
	  std::is_convertible_v<char const( & )[4], daw::sv2::string_view> );
	auto a = daw::sv2::string_view( "Hello" );
	(void)a;
	daw::sv2::string_view b;
	b = "Hello";
	(void)b;
	daw_expecting( daw::ensure_same_at_ct( ), true );
	daw::daw_string_view_constexpr_001( );
	daw::daw_string_view_find_last_of_001( );
	daw::daw_string_view_find_last_of_002( );
	daw::daw_string_view_find_last_of_003( );
	daw::daw_string_view_find_last_of_004( );
	daw::sv2::string_view arg = "";
	(void)arg;
	daw::daw_string_view_find_last_of_005( arg );
	daw::sv2::string_view arg2 = "a";
	(void)arg2;
	daw::daw_string_view_find_last_of_006( arg2 );
	daw::daw_string_view_find_last_of_007( arg2 );
	daw::daw_string_view_find_first_of_if_001( );
	daw::daw_string_view_find_first_of_if_002( );
	daw::daw_string_view_find_first_of_if_003( );
	daw::daw_string_view_find_first_not_of_if_001( );
	daw::daw_string_view_find_first_not_of_if_002( );
	daw::daw_string_view_find_first_not_of_if_003( );
	daw::daw_string_view_find_first_of_001( );
	daw::daw_string_view_find_first_of_002( );
	daw::daw_string_view_find_first_of_003( );
	daw::daw_string_view_find_first_of_004( );
	daw::daw_string_view_find_first_not_of_001( );
	daw::daw_string_view_find_first_not_of_002( );
	daw::daw_string_view_find_last_not_of_001( );
	daw::daw_string_view_find_last_not_of_002( );
	daw::daw_string_view_find_last_not_of_003( );
	daw::daw_string_view_find_last_not_of_004( );
	daw::daw_string_view_find_last_not_of_005( );
	daw::daw_string_view_find_last_not_of_if_001( );
	daw::daw_string_view_find_last_not_of_if_002( );
	daw::daw_string_view_find_last_not_of_if_003( );
	daw::daw_string_view_find_last_not_of_if_004( );
	daw::daw_string_view_find_last_not_of_if_005( );
	daw::daw_string_view_search_001( );
	daw::daw_string_view_search_002( );
	daw::daw_string_view_search_003( );
	daw::daw_string_view_search_last_001( );
	daw::daw_string_view_search_last_002( );
	daw::daw_string_view_search_last_003( );
	daw::daw_string_view_search_last_004( );
	daw::daw_string_view_search_last_005( );
	daw::tc001( );
	daw::tc002( );
	daw::tc003( );
	daw::tc004( );
	daw::tc004capacity( );
	daw::tc005capacity( );
	daw::tc006capacity( );
	daw::tc007accessor( );
	daw::tc008accessor( );
	daw::tc009accessor( );
	daw::tc010accessor( );
	daw::tc011accessor( );
	daw::tc012accessor( );
	daw::tc013modifier( );
	daw::tc014modifier( );
	daw::tc015modifier( );
	daw::tc016conversion( );
	daw::tc017conversion( );
	daw::tc018operation( );
	daw::tc019operation( );
	daw::tc020comparison( );
	daw::tc021comparison( );
	daw::tc022comparison( );
	daw::daw_can_be_string_view_starts_with_001( );
	daw::daw_can_be_string_view_starts_with_002( );
	daw::daw_can_be_string_view_starts_with_003( );
	daw::daw_can_be_string_view_starts_with_004( );
	daw::daw_can_be_string_view_starts_with_005( );
	daw::daw_can_be_string_view_starts_with_006( );
	daw::daw_can_be_string_view_starts_with_007( );
	daw::daw_can_be_string_view_starts_with_008( );
	daw::daw_can_be_string_view_starts_with_009( );
	daw::daw_can_be_string_view_ends_with_001( );
	daw::daw_can_be_string_view_ends_with_002( );
	daw::daw_can_be_string_view_ends_with_003( );
	daw::daw_can_be_string_view_ends_with_004( );
	daw::daw_can_be_string_view_ends_with_005( );
	daw::daw_can_be_string_view_ends_with_006( );
	daw::daw_can_be_string_view_ends_with_007( );
	daw::daw_can_be_string_view_ends_with_008( );
	daw::daw_can_be_string_view_ends_with_009( );
	daw::daw_can_be_string_view_ends_with_010( );
	daw::daw_pop_front_test_001( );
	daw::daw_pop_front_count_test_001( );
	daw::daw_pop_front_until_sv_test_001( );
	daw::daw_pop_back_count_test_001( );
	daw::daw_pop_back_until_sv_test_001( );
	daw::daw_pop_back_until_sv_test_002( );
	daw::daw_pop_front_pred_test_001( );
	daw::daw_pop_back_pred_test_001( );
	daw::daw_try_pop_back_until_sv_test_001( );
	daw::daw_try_pop_back_until_sv_test_002( );
	daw::daw_try_pop_front_until_sv_test_001( );
	daw::daw_try_pop_front_until_sv_test_002( );
	daw::daw_diff_assignment_001( );
	daw::daw_literal_test_001( );
	daw::daw_stdhash_test_001( );
	daw::daw_rfind_test_001( );
	daw::daw_rfind_test_002( );
	daw::daw_rfind_test_003( );
	daw::daw_find_test_001( );
	daw::daw_find_test_002( );
	daw::daw_find_test_003( );
	daw::daw_test_any_char_001( );
	daw::daw_remove_prefix_num_test_001( );
	daw::daw_remove_prefix_num_test_002( );
	daw::daw_remove_prefix_num_test_003( );
	daw::daw_remove_prefix_until_test_001( );
	daw::daw_remove_prefix_until_test_002( );
	daw::daw_remove_prefix_until_test_003( );
	daw::daw_remove_prefix_until_test_004( );
	daw::daw_remove_suffix_while_test_001( );
	daw::daw_remove_suffix_while_test_002( );
	daw::daw_arbutrary_string_view_list_001( );
	daw::daw_trim_prefix_test_001( );
	daw::daw_trim_prefix_test_002( );
	daw::daw_trim_suffix_test_001( );
	daw::daw_trim_suffix_test_002( );
	daw::daw_construct_from_string_001( );
	daw::daw_construct_from_string_002( );
	daw::daw_construct_from_string_view_001( );
	daw::daw_construct_from_string_view_002( );
	daw::daw_construct_from_string_literal_001( );
	daw::daw_construct_from_string_literal_002( );
	daw::daw_convert_to_std_string( );
	daw::daw_convert_to_std_string_view( );
	daw::daw_from_charptr( );
#if defined( __cpp_char8_t )
	daw::daw_from_char8ptr( );
#endif
	daw::daw_trim( );
	daw::daw_trim_copy( );
	daw::daw_array_ctad( );
	daw::daw_find_first_match( );
#if defined( DAW_HAS_CPP20_3WAY_COMPARE )
	daw::daw_3way_compare_test_001( );
#endif
}
#if defined( DAW_USE_EXCEPTIONS )
catch( std::exception const &ex ) {
	std::cerr << "An unexpected exception was thrown: " << typeid( ex ).name( )
	          << '\n'
	          << ex.what( ) << '\n'
	          << std::flush;
	exit( 1 );
} catch( ... ) {
	std::cerr << "An unknown exception was thrown\n" << std::flush;
	throw;
}
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_parser_addons.h"
#include "daw/daw_parser_helper.h"
#include "daw/daw_string_view.h"
#include "daw/daw_utility.h"

#include <cstdint>
#include <iostream>
#include <string>

void daw_parser_helper001( ) {
	static std::string uint_test = "43453";
	uint32_t ui32 = 0;
	int32_t i32 = 0;
	daw::parser::parse_unsigned_int( std::data( uint_test ), daw::data_end( uint_test ), ui32 );
	std::cout << "parse_unsigned_int: uint32 parser test from: '" << uint_test
	          << "' to " << ui32 << '\n';
	daw::expecting_message( 43453 == ui32, "Value did not parse correctly" );
	std::cout << std::endl;
	daw::parser::parse_int( std::data( uint_test ), daw::data_end( uint_test ), i32 );
	std::cout << "parse_int: int32 parser test from: '" << uint_test << "' to "
	          << i32 << '\n';
	daw::expecting_message( 43453 == i32, "Value did not parse correctly" );

	static std::string int_test = "-34534";
	i32 = 0;
	daw::parser::parse_int( std::data( int_test ), daw::data_end( int_test ), i32 );
	std::cout << "parse_int: int32 parser test from: '" << int_test << "' to "
	          << i32 << '\n';
	daw::expecting_message( -34534 == i32, "Value did not parse correctly" );

	static std::string tr_test = "  hello  ";
	std::cout << "Test trimming '" << tr_test << "'\n";

	auto tr_left =
	  daw::parser::trim_left( std::data( tr_test ), daw::data_end( tr_test ) ).to_string( );
	std::cout << "Left only '" << tr_left << "'\n";
	daw::expecting_message( tr_left == "hello  ", "Trim left did not work" );

	auto tr_right =
	  daw::parser::trim_right( std::data( tr_test ), daw::data_end( tr_test ) ).to_string( );
	std::cout << "Right only '" << tr_right << "'\n";
	daw::expecting_message( tr_right == "  hello", "Trim right did not work" );

	auto tr_full =
	  daw::parser::trim( std::data( tr_test ), daw::data_end( tr_test ) ).to_string( );
	std::cout << "Full trim '" << tr_full << "'\n";
	daw::expecting_message( tr_full == "hello", "Trim did not work" );

	static std::string empty;
	auto empty_left =
	  daw::parser::trim_left( std::data( empty ), daw::data_end( empty ) ).to_string( );
	std::cout << "Empty Left only '" << empty_left << "'\n";
	daw::expecting_message( empty_left == "", "Trim left did not work" );

	auto empty_right =
	  daw::parser::trim_right( std::data( empty ), daw::data_end( empty ) ).to_string( );
	std::cout << "Empty Right only '" << empty_right << "'\n";
	daw::expecting_message( empty_right == "", "Trim right did not work" );

	auto empty_full =
	  daw::parser::trim( std::data( empty ), daw::data_end( empty ) ).to_string( );
	std::cout << "Empty full trim '" << empty_full << "'\n";
	daw::expecting_message( empty_full == "", "Trim did not work" );
}

void daw_parser_helper002( ) {
	// skip ws
	daw::string_view const test_str = "          this is a test";
	auto const sws =
	  daw::parser::skip_ws( std::data( test_str ), daw::data_end( test_str ) ).to_string( );
	std::cout << "With leading ws '" << test_str.to_string( ) << "', without '"
	          << sws << "'\n";
}

int main( ) {
	daw_parser_helper001( );
	daw_parser_helper002( );
}

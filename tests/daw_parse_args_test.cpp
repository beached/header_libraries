// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_ensure.h>
#include <daw/daw_parse_args.h>

#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>

static_assert(
  std::is_same_v<
    typename std::iterator_traits<daw::argv_iterator>::iterator_category,
    std::random_access_iterator_tag> );
static_assert(
  std::is_same_v<
    typename std::iterator_traits<daw::args_iterator>::iterator_category,
    std::random_access_iterator_tag> );

namespace {
	[[nodiscard]] daw::Argument parse_argument( char *text ) {
		char program[] = "program";
		char *argv[] = { program, text };
		return daw::Arguments( 2, argv )[0];
	}

	void ensure_unsigned( char *text, std::optional<std::uint64_t> expected ) {
		daw_ensure( parse_argument( text ).as_int<std::uint64_t>( ) == expected );
	}

	void ensure_signed( char *text, std::optional<std::int64_t> expected ) {
		daw_ensure( parse_argument( text ).as_int<std::int64_t>( ) == expected );
	}
} // namespace

int main( ) {
	{
		char argv0[] = "program";
		char argv1[] = R"(--config="/home/foo/config.json")";
		char *argv[] = { argv0, argv1 };
		auto parsed_args = daw::Arguments( 2, argv );
		daw_ensure( parsed_args.size( ) == 1 );
		daw_ensure( parsed_args.program_name( ) == "program" );
		daw_ensure( parsed_args[0].name == "config" );
		daw_ensure( parsed_args[0].value == "/home/foo/config.json" );
		daw_ensure( parsed_args["config"] );
		daw_ensure( *parsed_args["config"] == "/home/foo/config.json" );
		daw_ensure( not parsed_args["missing"] );
		daw_ensure( parsed_args.first_named_argument( ) == 0U );
		daw_ensure( parsed_args.has_named_argument( ) );
	}

	{
		char argv0[] = "program";
		char argv1[] = "positional";
		char argv2[] = "--flag";
		char argv3[] = "--empty=";
		char argv4[] = R"(--quoted="a b")";
		char *argv[] = { argv0, argv1, argv2, argv3, argv4 };

		auto raw_args = daw::argv_view( 5, argv );
		daw_ensure( raw_args.size( ) == 5 );
		daw_ensure( raw_args[0] == "program" );
		daw_ensure( raw_args.end( ) - raw_args.begin( ) == 5 );
		daw_ensure( *( raw_args.begin( ) + 2 ) == "--flag" );

		auto argument_values = daw::args_view( 4, argv + 1 );
		daw_ensure( argument_values.size( ) == 4 );
		daw_ensure( argument_values[0].name.empty( ) );
		daw_ensure( argument_values[0].value == "positional" );
		daw_ensure( ( argument_values.begin( ) + 1 )->name == "flag" );

		auto parsed_args = daw::Arguments( 5, argv );
		daw_ensure( parsed_args.size( ) == 4 );
		daw_ensure( parsed_args[0].value == "positional" );
		daw_ensure( parsed_args[1].name == "flag" );
		daw_ensure( parsed_args[1].value.empty( ) );
		daw_ensure( parsed_args[2].name == "empty" );
		daw_ensure( parsed_args[2].value.empty( ) );
		daw_ensure( parsed_args[3].name == "quoted" );
		daw_ensure( parsed_args[3].value == "a b" );
		daw_ensure( parsed_args.first_named_argument( ) == 1U );
		daw_ensure( parsed_args.has_named_argument( ) );
		daw_ensure( parsed_args.find_argument_position( "quoted" ) == 3U );
	}

	{
		auto empty = daw::Arguments( 0, nullptr );
		daw_ensure( empty.empty( ) );
		daw_ensure( empty.program_name( ).empty( ) );

		char argv0[] = "program";
		char *argv[] = { argv0 };
		auto program_only = daw::Arguments( 1, argv );
		daw_ensure( program_only.empty( ) );
		daw_ensure( program_only.program_name( ) == "program" );
		daw_ensure( not program_only.first_named_argument( ) );
		daw_ensure( not program_only.has_named_argument( ) );
	}

	{
		char argv0[] = "program";
		char original[] = "--name=original";
		char changed[] = "--name=changed";
		char *argv[] = { argv0, original };
		auto parsed_args = daw::Arguments( 2, argv );
		daw_ensure( parsed_args[0].value == "original" );
		argv[1] = changed;
		daw_ensure( parsed_args[0].value == "changed" );
	}

	{
		char text[] = "text";
		daw_ensure( parse_argument( text ).as_string( ) == std::string( "text" ) );

		char zero[] = "0";
		char one[] = "1";
		char leading_zeroes[] = "000000000000000000000000000000000000001";
		char unsigned_max[] = "18446744073709551615";
		char unsigned_overflow[] = "18446744073709551616";
		char unsigned_huge[] = "999999999999999999999999999999999999999";
		char negative_unsigned[] = "-1";
		char invalid_unsigned[] = "12x";
		char empty_value[] = "";
		ensure_unsigned( zero, std::uint64_t{ 0 } );
		ensure_unsigned( one, std::uint64_t{ 1 } );
		ensure_unsigned( leading_zeroes, std::uint64_t{ 1 } );
		ensure_unsigned( unsigned_max,
		                 ( std::numeric_limits<std::uint64_t>::max )( ) );
		ensure_unsigned( unsigned_overflow, std::nullopt );
		ensure_unsigned( unsigned_huge, std::nullopt );
		ensure_unsigned( negative_unsigned, std::nullopt );
		ensure_unsigned( invalid_unsigned, std::nullopt );
		ensure_unsigned( empty_value, std::nullopt );

		char signed_max[] = "9223372036854775807";
		char signed_overflow[] = "9223372036854775808";
		char negative_one[] = "-1";
		char signed_min[] = "-9223372036854775808";
		char signed_underflow[] = "-9223372036854775809";
		char minus_only[] = "-";
		char invalid_signed[] = "12x";
		ensure_signed( zero, std::int64_t{ 0 } );
		ensure_signed( one, std::int64_t{ 1 } );
		ensure_signed( signed_max, ( std::numeric_limits<std::int64_t>::max )( ) );
		ensure_signed( signed_overflow, std::nullopt );
		ensure_signed( negative_one, std::int64_t{ -1 } );
		ensure_signed( signed_min, ( std::numeric_limits<std::int64_t>::min )( ) );
		ensure_signed( signed_underflow, std::nullopt );
		ensure_signed( minus_only, std::nullopt );
		ensure_signed( invalid_signed, std::nullopt );
		ensure_signed( empty_value, std::nullopt );
	}
}

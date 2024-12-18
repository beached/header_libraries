// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_ensure.h>
#include <daw/daw_parse_args.h>

int main( ) {
	{
		int argc = 2;
		char argv0[] = "program";
		char argv1[] = R"(--config="/home/foo/config.json")";
		char *argv[] = { argv0, argv1 };
		auto parsed_args = daw::Arguments( argc, argv );
		daw_ensure( parsed_args.size( ) == 1 );
		daw_ensure( parsed_args[0].name == "config" );
		daw_ensure( parsed_args[0].value == "/home/foo/config.json" );
	}

	{
		int argc = 2;
		char argv0[] = "program";
		char argv1[] = "--config=/home/foo/config.json";
		char *argv[] = { argv0, argv1 };
		auto parsed_args = daw::Arguments( argc, argv );
		daw_ensure( parsed_args.size( ) == 1 );
		daw_ensure( parsed_args[0].name == "config" );
		daw_ensure( parsed_args[0].value == "/home/foo/config.json" );
	}
}
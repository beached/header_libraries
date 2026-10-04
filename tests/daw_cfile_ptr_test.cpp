// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_cfile_ptr.h"
#include "daw/daw_ensure.h"

#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>

int main( int, char **argv ) {
	static_assert( std::is_nothrow_destructible_v<daw::unique_file_ptr> );
	static_assert( std::is_nothrow_move_constructible_v<daw::unique_file_ptr> );
	static_assert( not std::is_copy_constructible_v<daw::unique_file_ptr> );

	auto const file_name = std::string( argv[0] ) + ".cfile_ptr_test";
	auto const path_storage = file_name + ".ignored";
	auto const path = daw::string_view( path_storage.data( ), file_name.size( ) );
	constexpr char contents[] = "daw::unique_file_ptr\n";

	(void)std::remove( file_name.c_str( ) );
	daw_ensure( not daw::open_cfile( path, "rb" ) );

	{
		auto file = daw::open_cfile( path, "wb" );
		daw_ensure( file );
		auto moved_file = std::move( file );
		daw_ensure( not file );
		daw_ensure( moved_file );
		auto const count =
		  std::fwrite( contents, 1, sizeof( contents ) - 1, moved_file.get( ) );
		daw_ensure( count == sizeof( contents ) - 1 );
	}

	{
		auto file = daw::open_cfile( path, "rb" );
		daw_ensure( file );
		auto buffer = std::string( sizeof( contents ) - 1, '\0' );
		auto const count =
		  std::fread( buffer.data( ), 1, buffer.size( ), file.get( ) );
		daw_ensure( count == buffer.size( ) );
		daw_ensure( buffer == contents );
	}

	daw_ensure( std::remove( file_name.c_str( ) ) == 0 );
}

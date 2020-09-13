// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_memory_mapped_file.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>

template<typename String>
void create_file( String &&str ) {
	std::ofstream fs{ std::forward<String>( str ) };
	if( !fs ) {
		return;
	}
	fs << "This is a test\n";
}

void daw_memory_mapped_file_001( std::string const &file_name ) {
	create_file( file_name );
	daw::filesystem::memory_mapped_file_t<std::uint8_t> test(
	  static_cast<std::string_view>( file_name ) );
}

int main( ) {
	(void)daw_memory_mapped_file_001( "./blah.txt" );
}

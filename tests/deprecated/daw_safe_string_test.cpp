// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/deprecated/daw_safe_string.h"

#include <iostream>

void daw_safe_string_test_01( ) {
	auto s =
	  daw::make_safe_string( "This is an \\ \"unsafe string", []( auto un ) {
		  for( auto &c : un ) {
			  switch( c ) {
			  case '"':
			  case '\\':
				  c = '#';
				  break;
			  default:
				  break;
			  }
		  }
		  return un;
	  } );
	std::cout << s.get( ) << std::endl;
}

int main( ) {
	daw_safe_string_test_01( );
}

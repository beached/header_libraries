// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_static_warn_if.h>

template<typename T>
void warn_for_large_type( ) {
	daw_static_warn_if( sizeof( T ) > 8, "large type warning" );
}

int main( ) {
	daw_static_warn_if( false, "false condition warning" );
	warn_for_large_type<char>( );

	daw_static_warn_if( true, "true condition warning" );
	warn_for_large_type<char[16]>( );
}

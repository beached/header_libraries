// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ascii.h>
#include <daw/daw_ensure.h>

int main( ) {
	daw_ensure( daw::is_ascii_alpha( 'a' ) );
	daw_ensure( not daw::is_ascii_alpha( '0' ) );
	daw_ensure( daw::is_ascii_digit( '0' ) );
	daw_ensure( not daw::is_ascii_digit( 'a' ) );
	daw_ensure( daw::is_ascii_alphanum( 'a' ) );
	daw_ensure( not daw::is_ascii_alphanum( ' ' ) );
	daw_ensure( daw::is_ascii_printable( '!' ) );
}
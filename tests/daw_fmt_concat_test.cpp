// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_fmt_concat.h>

#include <iterator>
#include <string>
#include <string_view>

int main( ) {
	daw_ensure( daw::fmt_concat( ) == "" );
	daw_ensure( daw::fmt_concat( "The answer is ", 42, '.' ) ==
	            "The answer is 42." );

	std::string_view const hello = "hello";
	daw_ensure( daw::fmt_concat( hello, ' ', std::string{ "world" } ) ==
	            "hello world" );

	std::string result = "Values: ";
	auto out = daw::fmt_concat_to( std::back_inserter( result ), 1, ", ", 2 );
	*out = '!';
	daw_ensure( result == "Values: 1, 2!" );
}

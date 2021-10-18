// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_tuple2.h>

int main( int argc, char ** ) {
	auto a = daw::tuple2{ 1, 2.f, 3.0, 6 };
	auto b = daw::tuple2{ '4', "5", argc };

	auto big = daw::tuple2_cat(
	  a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a,
	  b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b,
	  a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a,
	  b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b, a, b );


	return daw::get<6>( big ) == argc ? 0 : 1;
}

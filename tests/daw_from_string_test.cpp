// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_from_string.h>
#include <daw/daw_string_view.h>

#include <string>

int main( ) {
	using namespace daw::string_view_literals;
	static constexpr auto sv0 = "1234"_sv;
	static_assert( daw::from_string( daw::tag<int>, sv0 ) == 1234 );
	static_assert(
		daw::from_string( daw::tag<daw::string_view>, sv0 ) == "1234"_sv );

	daw_ensure( daw::from_string( daw::tag<double>, sv0 ) == 1234.0 );
	daw_ensure( daw::from_string( daw::tag<std::string>, sv0 ) == "1234"_sv );
}
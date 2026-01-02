// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_span_writer.h"

#include <daw/daw_attributes.h>
#include <daw/daw_do_not_optimize.h>
#include <daw/daw_string_view.h>

#include <array>
#include <cstddef>
#include <span>

DAW_ATTRIB_NOINLINE void foo( std::array<char, 256> &buff,
                              std::size_t &out_size ) {
	auto sp = std::span<char>( buff );

	sp = daw::span_writer_ntz( sp, "Hello World.......Hello little letters:" );
	sp = daw::span_writer( sp, 'a', 'b', 'c', 0 );
	constexpr auto const expected_sz =
	  ( daw::string_view( "Hello World.......Hello little letters:" ).size( ) +
	    4 /*second span_writer*/ );
	daw_ensure( sp.size( ) == ( buff.size( ) - expected_sz ) );
	out_size = sp.size( );
}

int main( ) {
	auto buff = std::array<char, 256>( );
	std::size_t out_size = 0;
	foo( buff, out_size );
	daw::do_not_optimize( buff );
	daw::do_not_optimize( out_size );
}
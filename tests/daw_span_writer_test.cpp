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

DAW_ATTRIB_NOINLINE void test1_impl( std::array<char, 256> &buff,
                                     std::size_t &out_size ) {
	auto sp = daw::span_writer_ntz<char>(
	  buff, "Hello World.......Hello little letters:" );
	sp = daw::span_writer( sp, 'a', 'b', 'c', 0 );
	constexpr auto const expected_sz =
	  buff.size( ) -
	  ( ( daw::string_view( "Hello World.......Hello little letters:" ).size( ) +
	      4 /*second span_writer*/ ) );
	daw_ensure( sp.size( ) == expected_sz );
	out_size = sp.size( );
}

void test1( ) {
	auto buff = std::array<char, 256>( );
	std::size_t out_size = 0;
	test1_impl( buff, out_size );
	daw::do_not_optimize( buff );
	daw::do_not_optimize( out_size );
}

DAW_ATTRIB_NOINLINE void test2_impl( std::array<char, 256> &buff,
                                     std::size_t &out_size ) {
	auto sp = daw::output_span( buff );
	sp.write_ntz( "Hello World.......Hello little letters:" );
	sp.write( 'a', 'b', 'c', 0 );
	constexpr auto const expected_sz =
	  ( daw::string_view( "Hello World.......Hello little letters:" ).size( ) +
	    4 /*second span_writer*/ );
	daw_ensure( sp.size( ) == ( buff.size( ) - expected_sz ) );
	out_size = sp.size( );
}

void test2( ) {
	auto buff = std::array<char, 256>( );
	std::size_t out_size = 0;
	test2_impl( buff, out_size );
	daw::do_not_optimize( buff );
	daw::do_not_optimize( out_size );
}

int main( ) {
	test1( );
	test2( );
}
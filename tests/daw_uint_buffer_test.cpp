// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_uint_buffer.h"

#include <cassert>
#include <cstring>

std::uint64_t to_uint64( char const *ptr ) {
	std::uint64_t result;
	memcpy( &result, ptr, sizeof( std::uint64_t ) );
	return result;
}

std::uint32_t to_uint32( char const *ptr ) {
	std::uint32_t result;
	memcpy( &result, ptr, sizeof( std::uint32_t ) );
	return result;
}

std::uint16_t to_uint16( char const *ptr ) {
	std::uint16_t result;
	memcpy( &result, ptr, sizeof( std::uint16_t ) );
	return result;
}

int main( ) {
	assert( static_cast<std::uint64_t>( daw::to_uint64_buffer( "0123456789" ) ) ==
	        to_uint64( "0123456789" ) );
	assert( static_cast<std::uint32_t>( daw::to_uint32_buffer( "0123456789" ) ) ==
	        to_uint32( "0123456789" ) );
	assert( static_cast<std::uint16_t>( daw::to_uint16_buffer( "0123456789" ) ) ==
	        to_uint16( "0123456789" ) );

	{
		auto const a = daw::to_uint64_buffer( "0123456789" );
		auto const b = daw::to_uint64_buffer( "9876543210" );
		auto c = a & b;
		(void)c;
	}
	{
		auto const a = daw::to_uint32_buffer( "0123456789" );
		auto const b = daw::to_uint32_buffer( "9876543210" );
		auto c = a & b;
		(void)c;
	}
	{
		auto const a = daw::to_uint16_buffer( "0123456789" );
		auto const b = daw::to_uint16_buffer( "9876543210" );
		auto c = a & b;
		(void)c;
	}
}

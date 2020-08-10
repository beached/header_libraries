// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

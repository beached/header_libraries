// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#define DAW_DEFAULT_SIGNED_CHECKING 0

#include "daw/daw_ensure.h"
#include "daw/daw_traits.h"
#include "daw/integers/daw_signed.h"

#include <initializer_list>

DAW_ATTRIB_NOINLINE int test_plus( std::initializer_list<daw::i32> const &vals,
                                   int expected ) {
	daw::i32 sum = 0;
	for( auto v : vals ) {
		sum += v;
	}
	daw_ensure( sum == expected );
	return sum.get( );
}

DAW_ATTRIB_NOINLINE int test_div( int first, int inc, int expected ) {
	daw::i32 x = first;
	x /= inc;
	daw_ensure( x == expected );
	return x.get( );
}

int main( ) {
	using namespace daw::integers::literals;

	bool has_overflow = false;
	bool has_div_by_zero = false;
	auto const error_handler =
	  [&]( daw::integers::SignedIntegerErrorType error_type ) {
		  switch( error_type ) {
		  case daw::integers::SignedIntegerErrorType::DivideByZero:
			  has_div_by_zero = true;
			  break;
		  case daw::integers::SignedIntegerErrorType::Overflow:
			  has_overflow = true;
			  break;
		  }
	  };
	daw::integers::register_signed_overflow_handler( error_handler );
	daw::integers::register_signed_div_by_zero_handler( error_handler );

	test_plus( { 55, 55, 55 }, 165 );
	test_div( 110, 2, 55 );

	daw::i8 y = 10;
	y *= 100_i8;
	daw_ensure( has_overflow );
	has_overflow = false;

	y /= 0_i8;
	daw_ensure( has_div_by_zero );
	has_div_by_zero = false;
	daw::integers::register_signed_div_by_zero_handler( );
	bool has_exception = false;
	try {
		y /= 0_i8;
	} catch( daw::integers::signed_integer_div_by_zero_exception const & ) {
		has_exception = true;
	}
	daw_ensure( has_exception );
	has_exception = false;

	auto x = y * 2;
	auto xb = x or y;
	daw_ensure( xb );
	auto z = x * y;
	(void)z;
	daw::integers::register_signed_overflow_handler( error_handler );
	{
		has_overflow = false;
		daw::i8 i0 = std::numeric_limits<std::int8_t>::max( );
		daw::i8 i1 = std::numeric_limits<std::int8_t>::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		daw::i16 i0 = std::numeric_limits<std::int16_t>::max( );
		daw::i16 i1 = std::numeric_limits<std::int16_t>::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		daw::i32 i0 = std::numeric_limits<std::int32_t>::max( );
		daw::i32 i1 = std::numeric_limits<std::int32_t>::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		daw::i64 i0 = std::numeric_limits<std::int64_t>::max( );
		daw::i64 i1 = std::numeric_limits<std::int64_t>::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
}

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#define DAW_DEFAULT_SIGNED_CHECKING 0

#include "daw/daw_ensure.h"
#include "daw/integers/daw_signed.h"

#include <initializer_list>

using namespace daw::integers::literals;

DAW_ATTRIB_NOINLINE int test_plus( std::initializer_list<daw::i32> const &vals,
                                   daw::i32 expected ) {
	auto sum = 0_i32;
	for( auto v : vals ) {
		sum += v;
	}
	daw_ensure( sum == expected );
	return sum.value( );
}

DAW_ATTRIB_NOINLINE daw::i32 test_div( daw::i32 first, daw::i32 inc,
                                       daw::i32 expected ) {
	first /= inc;
	daw_ensure( first == expected );
	return first;
}

DAW_ATTRIB_NOINLINE daw::i64 test_div( daw::i64 first, daw::i64 inc,
                                       daw::i64 expected ) {
	first /= inc;
	daw_ensure( first == expected );
	return first;
}

int main( ) {
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

	test_plus( { 55_i32, 55_i32, 55_i32 }, 165_i32 );
	test_div( 110_i32, 2_i32, 55_i32 );
	test_div( 110_i64, 2_i64, 55_i64 );

	auto y = 10_i8;
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
	(void)has_exception;

	auto x = y * 2;
	auto xb = x or y;
	daw_ensure( xb );
	auto z = x * y;
	(void)z;
	daw::integers::register_signed_overflow_handler( error_handler );
	daw::integers::register_signed_div_by_zero_handler( error_handler );
	{
		has_overflow = false;
		auto i0 = daw::i8::max( );
		auto i1 = daw::i8::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i16::max( );
		auto i1 = daw::i16::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i32::max( );
		auto i1 = daw::i32::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::max( );
		auto i1 = daw::i64::max( );
		i0 += i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i8::min( );
		auto i1 = daw::i8::max( );
		i0 -= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i16::min( );
		auto i1 = daw::i16::max( );
		i0 -= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i32::min( );
		auto i1 = daw::i32::max( );
		i0 -= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i16::min( );
		auto i1 = daw::i16::max( );
		i0 -= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i8::max( );
		auto i1 = daw::i8::max( );
		i0 *= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i16::max( );
		auto i1 = daw::i16::max( );
		i0 *= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i32::max( );
		auto i1 = daw::i32::max( );
		i0 *= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::max( );
		auto i1 = daw::i64::max( );
		i0 *= i1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::min( );
		i0 *= -1;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::min( );
		--i0;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::min( );
		i0--;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::max( );
		++i0;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::max( );
		i0++;
		daw_ensure( has_overflow );
	}
	{
		has_overflow = false;
		auto i0 = daw::i64::min( );
		--i0;
		daw_ensure( has_overflow );
	}
	{
		has_div_by_zero = false;
		auto i0 = daw::i64::min( );
		i0 /= 0;
		daw_ensure( has_div_by_zero );
	}
	{
		has_overflow = false;
		has_div_by_zero = false;
		has_exception = false;
		auto i0 = 5_i32;
		daw::i8 i1 = static_cast<daw::i8>( i0 );
		daw_ensure( not has_overflow );
		i0 = daw::i32::max( );
		daw::i8 i2 = static_cast<daw::i8>( i0 );
		daw_ensure( has_overflow );
		has_overflow = false;
		daw::i64 i3 = i2;
		daw_ensure( not has_overflow );
	}
}

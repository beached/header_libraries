// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <iostream>

#include "daw/daw_benchmark.h"
#include "daw/daw_natural.h"
#include "daw/daw_utility.h"

constexpr bool daw_natural_test_01( ) {
	daw::natural_t<int> a = 5;
	daw::expecting( a, 5 );
	return true;
}
static_assert( daw_natural_test_01( ) );

constexpr bool daw_natural_test_02( ) {
	daw::natural_t<int> a = 2;
	daw::natural_t<int> b = 2;
	daw::expecting( a, b );
	return true;
}
static_assert( daw_natural_test_02( ) );

void daw_natural_test_03( ) {
	daw::expecting_exception<daw::exception::arithmetic_exception>(
	  []( ) { daw::natural_t<int>( -5 ); } );
}

constexpr bool daw_natural_op_plus_01( ) {
	daw::natural_t<int> a = 1;
	daw::natural_t<int> b = 2;
	auto result = a + b;

	daw::expecting( result, 3 );
	return true;
}
static_assert( daw_natural_op_plus_01( ) );

constexpr bool daw_natural_op_plus_02( ) {
	daw::natural_t<int> a = 4;
	daw::natural_t<int> b = 2;
	a += b;
	daw::expecting( 6, a );
	return true;
}
static_assert( daw_natural_op_plus_02( ) );

constexpr bool daw_natural_op_min_01( ) {
	constexpr daw::natural_t<int> a = 1;
	constexpr daw::natural_t<int> b = 2;
	auto result = b - a;
	daw::expecting( 1, result );
	return true;
}
static_assert( daw_natural_op_min_01( ) );

void daw_natural_op_min_02( ) {
	constexpr daw::natural_t<int> a = 1;
	constexpr daw::natural_t<int> b = 2;
	daw::expecting_exception<daw::exception::arithmetic_exception>(
	  [&]( ) { Unused( a - b ); } );
	daw::expecting( 1, a );
	daw::expecting( 2, b );
}

constexpr bool daw_natural_op_min_03( ) {
	daw::natural_t<int> a = 4;
	daw::natural_t<int> b = 2;
	a -= b;
	daw::expecting( 2, a );
	return true;
}
static_assert( daw_natural_op_min_03( ) );

void daw_natural_op_min_04( ) {
	daw::natural_t<int> a = 1;
	constexpr daw::natural_t<int> b = 2;
	daw::expecting_exception<daw::exception::arithmetic_exception>(
	  [&]( ) { a -= b; } );
	daw::expecting( 1, a );
	daw::expecting( 2, b );
}

constexpr bool daw_natural_op_mul_01( ) {
	daw::natural_t<int> a = 5;
	daw::natural_t<int> b = 6;
	auto result = a * b;

	daw::expecting( 30, result );
	return true;
}
static_assert( daw_natural_op_mul_01( ) );

constexpr bool daw_natural_op_mul_02( ) {
	daw::natural_t<int> a = 4;
	daw::natural_t<int> b = 2;
	a *= b;
	daw::expecting( 8, a );
	return true;
}
static_assert( daw_natural_op_mul_02( ) );

constexpr bool daw_natural_op_div_01( ) {
	daw::natural_t<int> a = 30;
	daw::natural_t<int> b = 6;
	auto result = a / b;
	daw::expecting( 5, result );
	return true;
}
static_assert( daw_natural_op_div_01( ) );

constexpr bool daw_natural_op_div_02( ) {
	daw::natural_t<int> a = 4;
	daw::natural_t<int> b = 2;
	a /= b;
	daw::expecting( a, 2 );
	return true;
}
static_assert( daw_natural_op_div_02( ) );

constexpr bool daw_natural_op_mod_01( ) {
	daw::natural_t<int> a = 3;
	daw::natural_t<int> b = 2;
	auto result = a % b;
	daw::expecting( 1, result );
	return true;
}
static_assert( daw_natural_op_mod_01( ) );

constexpr bool daw_natural_op_mod_02( ) {
	daw::natural_t<int> a = 3;
	daw::natural_t<int> b = 2;
	a %= b;
	daw::expecting( 1, a );
	return true;
}
static_assert( daw_natural_op_mod_02( ) );

constexpr bool daw_natural_literal_01( ) {
	using namespace daw::literals;
	auto a = 134_N;
	auto b = 333_N;
	daw::expecting( 134U, a );
	daw::expecting( 333U, b );
	return true;
}
static_assert( daw_natural_literal_01( ) );

constexpr bool daw_natural_op_inc_01( ) {
	daw::natural_t<int> a = 3;
	++a;
	daw::expecting( 4, a );
	return true;
}
static_assert( daw_natural_op_inc_01( ) );

constexpr bool daw_natural_op_inc_02( ) {
	daw::natural_t<int> a = 3;
	daw::natural_t<int> b = a++;
	daw::expecting( a, 4 );
	daw::expecting( b, 3 );
	return true;
}
static_assert( daw_natural_op_inc_02( ) );

constexpr bool daw_natural_op_dec_01( ) {
	daw::natural_t<int> a = 3;
	--a;
	daw::expecting( 2, a );
	return true;
}
static_assert( daw_natural_op_dec_01( ) );

constexpr bool daw_natural_op_dec_02( ) {
	daw::natural_t<int> a = 3;
	daw::natural_t<int> b = a--;
	daw::expecting( 2, a );
	daw::expecting( 3, b );
	return true;
}
static_assert( daw_natural_op_dec_02( ) );

void daw_natural_op_dec_03( ) {
	daw::natural_t<int> a = 1;
	daw::expecting_exception<daw::exception::arithmetic_exception>(
	  [&]( ) { --a; } );
	daw::expecting( 1, a );
}

void daw_natural_op_dec_04( ) {
	daw::natural_t<int> a = 1;
	daw::natural_t<int> b( 2 );
	daw::expecting_exception<daw::exception::arithmetic_exception>(
	  [&]( ) { a--; } );
	daw::expecting( 1, a );
	daw::expecting( 2, b );
}

int main( ) {
	daw_natural_test_03( );
	daw_natural_op_min_02( );
	daw_natural_op_min_04( );
	daw_natural_op_dec_03( );
	daw_natural_op_dec_04( );
}

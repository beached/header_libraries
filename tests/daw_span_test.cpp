// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include "daw/boost_test.h"
#include "daw/daw_span.h"

// Test trait
static_assert( daw::is_daw_span_v<daw::span<int>> );
static_assert( daw::is_daw_span_v<daw::span<int const>> );
static_assert( !daw::is_daw_span_v<int> );

static_assert( !std::is_constructible_v<daw::span<int>, daw::span<int> const> );

constexpr bool copy_non_const_span() {
	daw::span<int> a{};
	daw::span<int> b = a;
	return b.data( ) == nullptr and b.size( ) == 0;
}
static_assert(copy_non_const_span() );

constexpr bool copy_span_of_const( ) {
	daw::span<int const> a{};
	daw::span<int const> b = a;
	return b.data( ) == nullptr and b.size( ) == 0;
}
static_assert( copy_span_of_const( ) );

constexpr bool copy_const_span_of_const( ) {
	daw::span<int const> const a{};
	daw::span<int const> b = a;
	return b.data( ) == nullptr and b.size( ) == 0;
}
static_assert( copy_const_span_of_const( ) );

// Ensure that one cannot convert or construct a non_const T from a const T span
static_assert( !std::is_constructible_v<daw::span<int>, daw::span<int const> const> );
static_assert( !std::is_convertible_v<daw::span<int const> const, daw::span<int>> );

static_assert( std::is_constructible_v<daw::span<int>, std::nullptr_t>, "Can construct a span from null");
static_assert( std::is_constructible_v<daw::span<int>, std::nullptr_t, size_t>, "Can construct a span from null and size");

BOOST_AUTO_TEST_CASE( daw_span_test_001 ) {
	constexpr auto const a = daw::span( "This is a test" );
	for( auto c : a ) {
		std::cout << c;
	}
	std::cout << '\n';

	int const barry[] = {1, 2, 3, 4, 5};
	auto const b = daw::span( barry );
	BOOST_REQUIRE_EQUAL( b.size( ), 5 );
}

BOOST_AUTO_TEST_CASE( daw_span_test_002 ) {
	std::array<int, 5> a = { 1, 2, 3, 4, 5 };
	daw::span<int> s_a( a.data( ), a.size( ) );
	auto const sc_a = s_a;
}

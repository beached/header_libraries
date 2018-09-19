// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "daw/cpp_17.h"
#include "daw/iterator/daw_indexed_iterator.h"

constexpr bool array_test_001( ) {
	std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::ibegin( numbers );
	return *f == 1;
}
static_assert( array_test_001, "" );

constexpr bool array_test_002( ) {
	std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::ibegin( numbers );
	auto l = daw::iend( numbers );
	return daw::distance( f, l ) == 7;
}
static_assert( array_test_002, "" );

constexpr bool array_test_003( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::ibegin( numbers );
	return *f == 1;
}
static_assert( array_test_003, "" );

constexpr bool array_test_004( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::ibegin( numbers );
	auto l = daw::iend( numbers );
	return daw::distance( f, l ) == 7;
}
static_assert( array_test_004, "" );

constexpr bool array_test_005( ) {
	std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::cibegin( numbers );
	return *f == 1;
}
static_assert( array_test_005, "" );

constexpr bool array_test_006( ) {
	std::array<int, 7> numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::cibegin( numbers );
	auto l = daw::ciend( numbers );
	return daw::distance( f, l ) == 7;
}
static_assert( array_test_006, "" );

constexpr bool array_test_007( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::cibegin( numbers );
	return *f == 1;
}
static_assert( array_test_007, "" );

constexpr bool array_test_008( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto f = daw::cibegin( numbers );
	auto l = daw::ciend( numbers );
	return daw::distance( f, l ) == 7;
}
static_assert( array_test_008, "" );

constexpr bool inc_test_001( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::cibegin( numbers );
	++it;
	return *it == 2;
}
static_assert( inc_test_001, "" );

constexpr bool inc_test_002( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::cibegin( numbers );
	it++;
	return *it == 2;
}
static_assert( inc_test_002, "" );

constexpr bool inc_test_003( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::cibegin( numbers );
	it += 2;
	return *it == 3;
}
static_assert( inc_test_003, "" );


constexpr bool dec_test_001( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::ciend( numbers );
	--it;
	return *it == 7;
}
static_assert( dec_test_001, "" );

constexpr bool dec_test_002( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::ciend( numbers );
	it--;
	return *it == 7;
}
static_assert( dec_test_002, "" );

constexpr bool dec_test_003( ) {
	std::array<int, 7> const numbers = {1, 2, 3, 4, 5, 6, 7};
	auto it = daw::ciend( numbers );
	it -= 2;
	return *it == 6;
}
static_assert( dec_test_003, "" );


struct A {
	constexpr bool t( ) const {
		return true;
	}
};

constexpr bool operator_arrow_001( ) {
	std::array<A, 5> as = {A{},A{},A{},A{},A{}};
	auto it = daw::ibegin( as );

	return it->t( );
}
static_assert( operator_arrow_001(), "" );

constexpr bool operator_arrow_002( ) {
	std::array<A, 5> const as = {A{},A{},A{},A{},A{}};
	auto it = daw::cibegin( as );

	return it->t( );
}
static_assert( operator_arrow_002(), "" );




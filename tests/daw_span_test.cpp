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

#include "daw/daw_span.h"

// Test trait
static_assert( daw::is_daw_span_v<daw::span<int>> );
static_assert( daw::is_daw_span_v<daw::span<int const>> );
static_assert( !daw::is_daw_span_v<int> );

static_assert( !std::is_constructible_v<daw::span<int>, daw::span<int> const> );

// Ensure that one cannot convert or construct a non_const T from a const T span
static_assert(
  !std::is_constructible_v<daw::span<int>, daw::span<int const> const> );
static_assert(
  !std::is_convertible_v<daw::span<int const> const, daw::span<int>> );

constexpr bool construct_from_nullptr_const( ) {
	daw::span<int const> const a( nullptr );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr_const( ) );

constexpr bool construct_from_nullptr_and_zero_const( ) {
	daw::span<int const> const a( nullptr, 0 );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr_and_zero_const( ) );

constexpr bool construct_from_nullptr_and_non_zero_const( ) {
	daw::span<int const> const a( nullptr, 5 );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr_and_non_zero_const( ) );

constexpr bool construct_from_nullptr( ) {
	daw::span<int> const a( nullptr );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr( ) );

constexpr bool construct_from_nullptr_and_zero( ) {
	daw::span<int> const a( nullptr, 0 );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr_and_zero( ) );

constexpr bool construct_from_nullptr_and_non_zero( ) {
	daw::span<int> const a( nullptr, 5 );
	return a.data( ) == nullptr and a.empty( ) and a.size_bytes( ) == 0;
}
static_assert( construct_from_nullptr_and_non_zero( ) );

constexpr bool construct_from_pointer_pointer_const( ) {
	std::array<int const, 5> arry = {1, 2, 3, 4, 5};
	daw::span<int const> a( arry.data( ), arry.data( ) + arry.size( ) );

	return a.data( ) == arry.data( ) and a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( construct_from_pointer_pointer_const( ) );

constexpr bool construct_from_pointer_pointer( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	daw::span<int> a( arry.data( ), arry.data( ) + arry.size( ) );

	return a.data( ) == arry.data( ) and a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( construct_from_pointer_pointer( ) );

constexpr bool construct_from_c_array_const( ) {
	int const arry[] = {1, 2, 3, 4, 5};
	daw::span<int const> a( arry );

	return a.data( ) == std::data( arry ) and a.size( ) == std::size( arry ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( construct_from_c_array_const( ) );

constexpr bool construct_from_c_array( ) {
	int arry[] = {1, 2, 3, 4, 5};
	daw::span<int> a( arry );

	return a.data( ) == std::data( arry ) and a.size( ) == std::size( arry ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( construct_from_c_array( ) );

constexpr bool construct_from_pointer_size_type_const( ) {
	int const arry[5] = {1, 2, 3, 4, 5};
	int const *ptr = arry;
	daw::span<int const> a( ptr, 5 );

	return a.data( ) == std::data( arry ) and a.size( ) == std::size( arry );
}
static_assert( construct_from_pointer_size_type_const( ) );

constexpr bool construct_from_pointer_size_type( ) {
	int arry[5] = {1, 2, 3, 4, 5};
	int *ptr = arry;
	daw::span<int> a( ptr, 5 );

	return a.data( ) == std::data( arry ) and a.size( ) == std::size( arry );
}
static_assert( construct_from_pointer_size_type( ) );

constexpr bool construct_from_container_const( ) {
	std::array<int const, 5> arry = {1, 2, 3, 4, 5};
	daw::span<int const> a( arry );

	return a.data( ) == arry.data( ) and a.size( ) == arry.size( );
}
static_assert( construct_from_container_const( ) );

constexpr bool construct_from_container( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	daw::span<int> a( arry );

	return a.data( ) == arry.data( ) and a.size( ) == arry.size( );
}
static_assert( construct_from_container( ) );

constexpr bool copy_non_const_span( ) {
	daw::span<int> a{};
	daw::span<int> b = a;
	return b.data( ) == nullptr and b.empty( ) and b.size_bytes( ) == 0;
}
static_assert( copy_non_const_span( ) );

constexpr bool copy_span_of_const( ) {
	daw::span<int const> a{};
	daw::span<int const> b = a;
	return b.data( ) == nullptr and b.empty( ) and b.size_bytes( ) == 0;
}
static_assert( copy_span_of_const( ) );

constexpr bool copy_const_span_of_const( ) {
	daw::span<int const> const a{};
	daw::span<int const> b = a;
	return b.data( ) == nullptr and b.empty( ) and b.size_bytes( ) == 0;
}
static_assert( copy_const_span_of_const( ) );

static_assert( !daw::span<int const>::has_mutable_pointer );
static_assert( daw::span<int>::has_mutable_pointer );

constexpr bool copy_to_mutable( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	daw::span<int> const a{arry};
	auto b = a.copy( );
	b[0] = 5;
	(void)b[0];
	return arry.front( ) == 5;
}
static_assert( copy_to_mutable( ) );

constexpr bool ctad_ptr_size_const( ) {
	std::array<int const, 5> arry = {1, 2, 3, 4, 5};
	daw::span a( arry.data( ), arry.size( ) );

	return !a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_size_const( ) );

constexpr bool ctad_ptr_size( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	daw::span a( arry.data( ), arry.size( ) );

	return a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_size( ) );

constexpr bool ctad_ptr_ptr_const_01( ) {
	std::array<int const, 5> arry = {1, 2, 3, 4, 5};
	int const *ptr1 = arry.data( );
	int const *ptr2 = arry.data( ) + arry.size( );
	daw::span a( ptr1, ptr2 );

	return !a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_ptr_const_01( ) );

constexpr bool ctad_ptr_ptr_const_02( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	int const *ptr1 = arry.data( );
	int *ptr2 = arry.data( ) + arry.size( );
	daw::span a( ptr1, ptr2 );

	return !a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_ptr_const_02( ) );

constexpr bool ctad_ptr_ptr_01( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	int *ptr1 = arry.data( );
	int *ptr2 = arry.data( ) + arry.size( );
	daw::span a( ptr1, ptr2 );

	return a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_ptr_01( ) );

constexpr bool ctad_ptr_ptr_02( ) {
	std::array<int, 5> arry = {1, 2, 3, 4, 5};
	int *ptr1 = arry.data( );
	int const *ptr2 = arry.data( ) + arry.size( );
	daw::span a( ptr1, ptr2 );

	return a.has_mutable_pointer and a.data( ) == arry.data( ) and
	       a.size( ) == arry.size( ) and
	       a.size_bytes( ) == ( a.size( ) * sizeof( int ) );
}
static_assert( ctad_ptr_ptr_02( ) );

int main( ) {
	return 0;
}

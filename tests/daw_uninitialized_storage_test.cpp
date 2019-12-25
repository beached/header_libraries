// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <daw/daw_benchmark.h>
#include <daw/daw_uninitialized_storage.h>

#include <array>
#include <cstddef>
#include <memory>
#include <type_traits>

void test_001( ) {
	daw::uninitialized_storage<std::array<size_t, 100>> str;
	str.construct( );
	auto result = str->size( );
	str.destruct( );
	str.construct( 100ULL );
	result -= ( *str )[0];
	daw::expecting( result == 0ULL );
}

void test_002( ) {
	daw::uninitialized_storage<std::unique_ptr<int>> uptr{};
	static_assert(
	  std::is_copy_constructible_v<decltype( uptr )> ==
	  std::is_copy_constructible_v<typename decltype( uptr )::value_type> );
	static_assert(
	  std::is_copy_assignable_v<decltype( uptr )> ==
	  std::is_copy_assignable_v<typename decltype( uptr )::value_type> );
	static_assert(
	  std::is_move_constructible_v<decltype( uptr )> ==
	  std::is_move_constructible_v<typename decltype( uptr )::value_type> );
	static_assert(
	  std::is_move_assignable_v<decltype( uptr )> ==
	  std::is_move_assignable_v<typename decltype( uptr )::value_type> );

	uptr.construct( std::make_unique<int>( 100 ) );
	daw::uninitialized_storage<std::unique_ptr<int>> uptr2{};
	uptr2.construct( uptr.rref( ) );
	auto const &uptrr = *uptr;
	auto const &uptr2r = *uptr2;
	::daw::expecting( !uptrr );
	::daw::expecting( uptr2r );
	daw::expecting( *( *uptr2 ) == 100 );
}

int main( ) {
	test_001( );
	test_002( );
	return 0;
}

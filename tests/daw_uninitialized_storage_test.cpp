// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_uninitialized_storage.h>

#include <daw/daw_benchmark.h>

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
	daw::uninitialized_storage<std::unique_ptr<int>> uptr{ };
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
	daw::uninitialized_storage<std::unique_ptr<int>> uptr2{ };
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

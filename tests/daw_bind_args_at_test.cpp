// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bind_args_at.h"

#include <iostream>
#include <type_traits>

constexpr int l( int a, int b ) {
	return a < b;
}

constexpr auto less100 = daw::bind_args_at<1>( &l, 100 );
constexpr auto less5_100 = daw::bind_args_at<0>( less100, 5 );

struct Display {
	template<typename Arg, typename... Args>
	inline void operator( )( Arg const &arg, Args const &...args ) const {
		auto const show_item = []( auto const &item ) { std::cout << ' ' << item; };
		std::cout << arg;
		(void)( ( show_item( args ), 0 ) | ... );
	}

	constexpr void operator( )( ) const {}
};

constexpr auto show_some = daw::bind_args_at<5>( Display{ }, "Hello", "there" );
static_assert( not std::is_invocable_v<decltype( show_some ), int, int> );
static_assert(
  std::is_invocable_v<decltype( show_some ), int, int, int, int, int> );

int main( ) {
	if( less5_100( ) ) {
		show_some( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
		// std::cout << "Good\n";
	}
}

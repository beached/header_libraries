// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_concepts.h>

#include <type_traits>

static_assert( daw::same_as<int, int> );

inline constexpr auto foo = []( daw::Fn<void()> auto && ) {};
inline constexpr auto foo_t0 = []( ) {};
inline constexpr auto foo_t1 = []( ) { return 42; };
inline constexpr auto foo_t2 = []( int ) { return 42; };

static_assert( std::is_invocable_v<decltype(foo), decltype(foo_t0)> );
static_assert( std::is_invocable_v<decltype(foo), decltype(foo_t1)> );
static_assert( not std::is_invocable_v<decltype(foo), decltype(foo_t2)> );


int main( ) {}

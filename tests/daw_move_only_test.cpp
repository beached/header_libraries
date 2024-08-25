// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_move_only.h>

#include <type_traits>

static_assert( not std::is_copy_constructible_v<daw::move_only<int>> );
static_assert( not std::is_copy_constructible_v<daw::move_only<void>> );
static_assert( not std::is_copy_assignable_v<daw::move_only<int>> );
static_assert( not std::is_copy_assignable_v<daw::move_only<void>> );
static_assert( std::is_constructible_v<daw::move_only<int>, int> );

int main( ) {}

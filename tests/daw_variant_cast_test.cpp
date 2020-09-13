// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_variant_cast.h"

#include <variant>

constexpr bool test_01( ) noexcept {
	std::variant<int, float> f = 5;

	return daw::variant_cast<int>( f ) == 5;
}

static_assert( test_01( ) );

constexpr bool test_02( ) noexcept {
	std::variant<int, float> f = 5;

	return daw::variant_cast<unsigned long long>( f ) == 5;
}

static_assert( test_02( ) );

int main( ) {}

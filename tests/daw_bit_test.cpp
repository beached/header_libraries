// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bit.h"

#include <cstdint>

static_assert( daw::get_left_mask<uint8_t>( 5 ) == 0b00000111 );
static_assert( daw::get_right_mask<uint8_t>( 5 ) == 0b11100000 );
static_assert( !daw::are_set( 0b0000'1000, 0b1000'000, 0b0000'0001 ) );
static_assert( daw::are_set( 0b0000'1000, 0b1000'000, 0b0000'0001,
                             0b0000'1000 ) );
static_assert( daw::are_set( 0b1, 1u ) );
static_assert( 0b0001'0000 == daw::set_bits( 0u, 4u ) );
static_assert( 0b0001'0001 == daw::set_bits( 0u, 0u, 4u ) );
static_assert( 0b1110'1111 == daw::unset_bits( 0b1111'1111, 4u ) );
static_assert( 0b1110'1110 == daw::unset_bits( 0b1111'1111, 0u, 4u ) );
static_assert( 0b0001'0101 == daw::get_bits( 0b1111'1111, 0u, 2u, 4u ) );

int main( ) {}

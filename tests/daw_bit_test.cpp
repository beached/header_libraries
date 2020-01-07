// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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

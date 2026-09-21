// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_arith_traits.h>

#include <cstdint>
#include <type_traits>

static_assert( daw::has_make_signed_v<unsigned> );
static_assert( not daw::has_make_signed_v<float> );
static_assert( std::is_same_v<daw::try_make_signed_t<unsigned>, int> );
static_assert( std::is_same_v<daw::try_make_signed_t<float>, float> );

static_assert(
  std::is_same_v<daw::next_wider_fast_t<std::uint8_t>, std::uint16_t> );
static_assert(
  std::is_same_v<daw::next_wider_fast_t<std::uint64_t>, std::uint64_t> );
static_assert( std::is_same_v<
               daw::widest_type_t<std::uint8_t, std::uint64_t, std::uint32_t>,
               std::uint64_t> );

int main( ) {}

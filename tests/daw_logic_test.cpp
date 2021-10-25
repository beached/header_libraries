// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_logic.h>

static_assert( daw::nsc_and( true, true, true ) );
static_assert( not daw::nsc_and( true, true, false ) );
static_assert( not daw::nsc_and( true, false, true ) );
static_assert( not daw::nsc_and( true, false, false ) );
static_assert( not daw::nsc_and( false, true, true ) );
static_assert( not daw::nsc_and( false, true, false ) );
static_assert( not daw::nsc_and( false, false, true ) );
static_assert( not daw::nsc_and( false, false, false ) );

static_assert( daw::nsc_or( true, true, true ) );
static_assert( daw::nsc_or( true, true, false ) );
static_assert( daw::nsc_or( true, false, true ) );
static_assert( daw::nsc_or( true, false, false ) );
static_assert( daw::nsc_or( false, true, true ) );
static_assert( daw::nsc_or( false, true, false ) );
static_assert( daw::nsc_or( false, false, true ) );
static_assert( not daw::nsc_or( false, false, false ) );

int main( ) {}

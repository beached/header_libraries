// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"
#include "iso646.h"

#include <exception>

namespace daw::ensure {
	[[noreturn]] DAW_ATTRIB_NOINLINE void do_error( ) {
		std::terminate( );
	}
} // namespace daw::ensure

#define daw_ensure( ... )         \
	do {                            \
		if( not( __VA_ARGS__ ) ) {    \
			::daw::ensure::do_error( ); \
		}                             \
	} while( false )

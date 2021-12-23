// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_move.h"

#define DAW_LIFT( ... )                                                        \
	[]( auto &&...args ) noexcept( noexcept( __VA_ARGS__( DAW_FWD(               \
	  args )... ) ) ) -> decltype( __VA_ARGS__( DAW_FWD( args )... ) ) {         \
		return __VA_ARGS__( DAW_FWD( args )... );                                  \
	}


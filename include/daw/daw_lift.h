// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "ciso646.h"
#include "daw_cpp_feature_check.h"
#include "daw_move.h"

// WORKAROUND: Older MSVC cannot see argsLIFT inside noexcept( exprn )
#if not defined( DAW_HAS_MSVC )
#define DAW_LIFT( ... )                                                        \
	[]( auto &&...argsLIFT ) noexcept( noexcept( __VA_ARGS__( DAW_FWD(           \
	  argsLIFT )... ) ) ) -> decltype( __VA_ARGS__( DAW_FWD( argsLIFT )... ) ) { \
		return (__VA_ARGS__)( DAW_FWD( argsLIFT )... );                            \
	}
#else
#define DAW_LIFT( ... )                                   \
	[]( auto &&...argsLIFT ) -> decltype( __VA_ARGS__(      \
	                           DAW_FWD( argsLIFT )... ) ) { \
		return (__VA_ARGS__)( DAW_FWD( argsLIFT )... );       \
	}
#endif

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries/
//

#pragma once

/// Explicitly do a non-short circuiting logical and with bitwise and
#define DAW_NSC_AND( Lhs, Rhs )                                                \
	static_cast<bool>( static_cast<bool>( Lhs ) & static_cast<bool>( Rhs ) )

/// Explicitly do a non-short circuiting logical or with bitwise or
#define DAW_NSC_OR( Lhs, Rhs )                                                 \
	static_cast<bool>( static_cast<bool>( Lhs ) | static_cast<bool>( Rhs ) )

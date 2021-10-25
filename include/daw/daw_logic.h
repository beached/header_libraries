// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries/
//

#pragma once

#include "daw_attributes.h"

namespace daw {
	/// Explicitly do a non-short circuiting logical and with bitwise and
	template<typename Bool, typename... Bools>
	DAW_ATTRIB_INLINE constexpr bool nsc_and( Bool lhs, Bools... rhs ) noexcept {
		return ( static_cast<bool>( rhs ) & ... & static_cast<bool>(lhs) );
	}

	/// Explicitly do a non-short circuiting logical or with bitwise or
	template<typename Bool, typename... Bools>
	DAW_ATTRIB_INLINE constexpr bool nsc_or( Bool lhs, Bools... rhs ) noexcept {
		return ( static_cast<bool>( rhs ) | ... | static_cast<bool>(lhs) );
	}
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_move.h"

namespace daw {
	template<typename To, typename From>
	[[nodiscard]] DAW_ATTRIB_INSTRINSIC DAW_ATTRIB_INLINE constexpr auto
	as( From &&from ) noexcept( noexcept( static_cast<To>( DAW_FWD( from ) ) ) )
	  -> decltype( static_cast<To>( DAW_FWD( from ) ) ) {
		return static_cast<To>( DAW_FWD( from ) );
	}
} // namespace daw

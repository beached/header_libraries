// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <utility>

namespace daw {
	template<typename T>
	DAW_ATTRIB_INLINE constexpr decltype( auto ) forward_lvalue( T &&r ) {
		if constexpr( std::is_lvalue_reference_v<T> ) {
			return std::forward<T>( r );
		} else {
			return static_cast<std::remove_reference_t<T>>( r );
		}
	}
} // namespace daw

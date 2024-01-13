// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_traits_is_same.h"

#include <cstdlib>

namespace daw {
	template<typename T>
	struct undefined_t;

	template<typename T>
	inline constexpr auto undefined_v = [] {
		if constexpr( not daw::is_same_v<T, undefined_t<T>> ) {
			std::abort( );
		} else {
			return static_cast<T *>( nullptr );
		}
	}( );
} // namespace daw

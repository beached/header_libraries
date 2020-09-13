// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <utility>

namespace daw {
	template<typename T, typename U = T>
	constexpr T exchange( T &obj, U &&new_value ) noexcept {
		T old_value = daw::move( obj );
		obj = std::forward<U>( new_value );
		return old_value;
	}
} // namespace daw

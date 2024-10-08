// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"
#if DAW_CPP_VERSION >= 202002L
#include <utility>
#else
#include "daw_attributes.h"
#include "daw_move.h"

#include <daw/stdinc/move_fwd_exch.h>
#endif

namespace daw {
#if DAW_CPP_VERSION >= 202002L
	using std::exchange;
#else
	template<typename T, typename U = T>
	DAW_ATTRIB_INLINE constexpr T exchange( T &obj, U &&new_value ) {
		T old_value = std::move( obj );
		obj = DAW_FWD( new_value );
		return old_value;
	}
#endif
} // namespace daw

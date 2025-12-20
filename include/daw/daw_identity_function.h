// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_move.h"

namespace daw {
	struct identity_function_t {
		explicit identity_function_t() = default;

		template<typename T>
		DAW_CPP23_STATIC_CALL_OP constexpr T &&
		operator( )( T &&value ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return DAW_FWD( value );
		}
	};

	inline constexpr auto identity_function =	identity_function_t{};
} // namespace daw

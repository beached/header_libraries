// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_consteval.h"
#include "daw_cpp_feature_check.h"

#include <cstdlib>

namespace daw {
	template<auto Value>
	struct constant {
		using type = decltype( Value );
		static constexpr type value = Value;

		DAW_CONSTEVAL operator type( ) const noexcept {
			return Value;
		}

		DAW_CPP23_STATIC_CALL_OP DAW_CONSTEVAL type operator( )( )
		  DAW_CPP23_STATIC_CALL_OP_CONST noexcept {
			return Value;
		}
	};

	template<auto Value>
	inline constexpr auto constant_v = constant<Value>{ };
} // namespace daw

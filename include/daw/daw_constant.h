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
#include "daw_parser_helper_sv.h"
#include "daw_string_view.h"

#include <cstdlib>

namespace daw {
	template<auto Value>
	struct constant {
		using daw_i_am_a_constant = void;
		using value_type = decltype( Value );
		using type = constant;
		static constexpr auto value = Value;

		DAW_CONSTEVAL operator value_type( ) const noexcept {
			return Value;
		}

		DAW_CPP23_STATIC_CALL_OP DAW_CONSTEVAL value_type operator( )( )
		  DAW_CPP23_STATIC_CALL_OP_CONST noexcept {
			return Value;
		}
	};

	template<decltype( auto ) Value>
	inline constexpr auto constant_v = constant<Value>{ };
} // namespace daw

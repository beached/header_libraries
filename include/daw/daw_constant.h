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

#if defined( DAW_HAS_CPP_CONSTEVAL )
	// TODO check for __cpp_user_defined_literal support if there is one with a
	// version since 2008
	template<char... Cs>
	consteval auto operator""_c( ) {
		constexpr char const str[] = { Cs..., '\0' };
		constexpr daw::string_view sv = str;
		constexpr auto result =
		  daw::parser::parse_unsigned_int<unsigned long long>( sv );
		return constant<result>{ };
	}
#endif
} // namespace daw

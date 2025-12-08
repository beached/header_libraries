// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_parser_addons.h"

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
	inline constexpr auto constant_v = constant<Value>{};

	namespace literals {
		template<char... c>
		DAW_CONSTEVAL auto operator""_c( ) {
			static_assert( sizeof...( c ) != 0 );
			constexpr auto buff_size = sizeof...( c ) + 1;
			constexpr char buff[buff_size] = {c...};
			using result_t = unsigned long long;
			constexpr auto result = parser::parse_unsigned_int<result_t>(
				buff );
			return daw::constant_v<result>;
		}
	}
} // namespace daw
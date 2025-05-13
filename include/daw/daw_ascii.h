// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/impl/daw_make_trait.h"

#include <type_traits>

namespace daw {
	struct is_ascii_digit_t {
		explicit is_ascii_digit_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr bool
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			// '0' <= c <= '9'
			return Integer{ 48 } <= c and c <= Integer{ 57 };
		}
	};
	inline constexpr auto is_ascii_digit = is_ascii_digit_t{ };

	struct is_ascii_alpha_t {
		explicit is_ascii_alpha_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr bool
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			// ( 'A' <= c <= 'Z' ) or ( 'a' <= c <= 'z' )
			return ( Integer{ 65 } <= c and c <= Integer{ 90 } ) or
			       ( Integer{ 97 } <= c and c <= Integer{ 122 } );
		}
	};
	inline constexpr auto is_ascii_alpha = is_ascii_alpha_t{ };

	struct is_ascii_alphanum_t {
		explicit is_ascii_alphanum_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr bool
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return is_ascii_digit( c ) or is_ascii_alpha( c );
		}
	};
	inline constexpr auto is_ascii_alphanum = is_ascii_alphanum_t{ };

	struct is_ascii_printable_t {
		explicit is_ascii_printable_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr bool
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return Integer{ 32 } < c and c < Integer{ 127 };
		}
	};
	inline constexpr auto is_ascii_printable = is_ascii_printable_t{ };

	struct to_upper_ascii_t {
		explicit to_upper_ascii_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr Integer
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if( Integer{ 97 } <= c and c <= Integer{ 122 } ) {
				return static_cast<Integer>( c - Integer{ 32 } );
			}
			return c;
		}
	};
	inline constexpr auto to_upper_ascii = to_upper_ascii_t{ };

	struct to_lower_ascii_t {
		explicit to_lower_ascii_t( ) = default;

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr Integer
		operator( )( Integer c ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if( Integer{ 65 } <= c and c <= Integer{ 90 } ) {
				return static_cast<Integer>( c + Integer{ 32 } );
			}
			return c;
		}
	};
	inline constexpr auto to_lower_ascii = to_lower_ascii_t{ };
} // namespace daw

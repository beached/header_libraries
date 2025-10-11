// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_string_view.h"
#include "daw/daw_utility.h"

#include <optional>
#include <charconv>

namespace daw::from_string_trait {
	template<typename T>
	struct from_string_t {};

	template<typename I>
		requires std::is_arithmetic_v<I>
	struct from_string_t<I> {
		explicit from_string_t( ) = default;

		DAW_CPP23_STATIC_CALL_OP constexpr std::optional<I> operator(
		)( daw::string_view str ) DAW_CPP23_STATIC_CALL_OP_CONST {
			I result = I{};
			std::from_chars_result fs_r = std::from_chars(
				str.data( ),
				str.data_end( ),
				result );
			if(fs_r.ec == std::errc( )) {
				return result;
			}
			return std::nullopt;
		}
	};

	namespace impl {
		template<typename String>
		concept StringConstructible = requires( char const *p, std::size_t sz )
		{
			String( p, sz );
		};
	}

	template<impl::StringConstructible String>
	struct from_string_t<String> {
		explicit from_string_t( ) = default;

		DAW_CPP23_STATIC_CALL_OP constexpr std::optional<String> operator(
		)( daw::string_view str ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return String( str.data( ), str.size( ) );
		}
	};
}

namespace daw {
	template<typename T>
		requires requires { daw::from_string_trait::from_string_t<T>{}; }
	constexpr auto from_string( daw::tag_t<T>, daw::string_view sv ) {
		return daw::from_string_trait::from_string_t<T>{}( sv );
	}
}
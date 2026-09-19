// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_20.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_move.h"

#include <array>
#include <concepts>
#include <format>
#include <string>
#include <type_traits>

#if DAW_CPP_VERSION >= 202302L and defined( __cpp_lib_format )
#if __cpp_lib_format >= 202110L
#define DAW_HAS_CPP23_FORMATTABLE 1
#endif
#endif

namespace daw::fmt_concat_details {
	template<typename T, typename CharT = char>
	concept formattable =
#if defined( DAW_CPP23_FORMATTABLE )
	  std::formattable<T, CharT>;
#else
	  std::is_default_constructible_v<std::formatter<std::decay_t<T>, CharT>> and
	  std::is_copy_constructible_v<std::formatter<std::decay_t<T>, CharT>> and
	  std::is_move_constructible_v<std::formatter<std::decay_t<T>, CharT>>;
#endif

	template<typename CharT, typename... Args>
	inline constexpr std::array concat_fmt_string_v = [] {
		constexpr std::size_t sz = sizeof...( Args ) * 2;
		auto result = std::array<CharT, sz + 1>{ };
		for( std::size_t n = 0; n < sz; n += 2 ) {
			result[n] = static_cast<CharT>( '{' );
			result[n + 1] = static_cast<CharT>( '}' );
		}
		return result;
	}( );

} // namespace daw::fmt_concat_details

namespace daw {
	template<typename CharT = char, fmt_concat_details::formattable<CharT>... Ts>
	inline std::basic_string<CharT> fmt_concat( Ts &&...args ) {
		return std::format(
		  std::basic_format_string<CharT, Ts...>(
		    fmt_concat_details::concat_fmt_string_v<CharT, Ts...>.data( ) ),
		  DAW_FWD( args )... );
	}

	template<typename CharT = char, typename OutputIt,
	         fmt_concat_details::formattable<CharT>... Ts>
	inline OutputIt fmt_concat_to( OutputIt out, Ts &&...args ) {
		return std::format_to(
		  out,
		  std::basic_format_string<CharT, Ts...>(
		    fmt_concat_details::concat_fmt_string_v<CharT, Ts...>.data( ) ),
		  DAW_FWD( args )... );
	}
} // namespace daw

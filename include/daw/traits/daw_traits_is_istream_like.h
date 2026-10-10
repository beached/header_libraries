// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if not defined( DAW_HAS_CPP20_CONCEPTS )
#include <daw/stdinc/declval.h>
#include <daw/stdinc/void_t.h>
#endif

namespace daw::traits {
#if defined( DAW_HAS_CPP20_CONCEPTS )
	template<typename T>
	inline constexpr bool is_istream_like_lite_v = requires( T const &stream ) {
		typename T::char_type;
		stream.good( );
		stream.width( );
		stream.flags( );
	};

	template<typename T, typename CharT>
	inline constexpr bool has_read_member_v = requires( T &stream ) {
		stream.read( static_cast<CharT *>( nullptr ), int{ } );
	};
#else
	template<typename, typename = void>
	inline constexpr bool is_istream_like_lite_v = false;

	template<typename T>
	inline constexpr bool is_istream_like_lite_v<
	  T, std::void_t<typename T::char_type,
	                 decltype( std::declval<T const &>( ).good( ) ),
	                 decltype( std::declval<T const &>( ).width( ) ),
	                 decltype( std::declval<T const &>( ).flags( ) )>> = true;

	template<typename T, typename CharT, typename = void>
	inline constexpr bool has_read_member_v = false;

	template<typename T, typename CharT>
	inline constexpr bool
	  has_read_member_v<T, CharT,
	                    std::void_t<decltype( std::declval<T &>( ).read(
	                      std::declval<CharT *>( ), std::declval<int>( ) ) )>> =
	    true;
#endif

	template<typename T, typename CharT>
	inline constexpr bool is_istream_like_v =
	  is_istream_like_lite_v<T> and has_read_member_v<T, CharT>;
} // namespace daw::traits

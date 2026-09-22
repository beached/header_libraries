// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <type_traits>

namespace daw::traits_is_sv {
	template<typename /*String*/, typename = void>
	inline constexpr bool is_string_view_like_v = false;

	template<typename S>
	inline constexpr bool is_string_view_like_v<
	  S, std::void_t<
	       decltype( ( (void)std::declval<S const &>( ).begin( ) ), /* has begin(
	                                                                   ) member */
	                 ( (void)std::declval<S const &>( ).end( ) ),   /* has end( )
	                                                                   member */
	                 ( (void)std::declval<S const &>( )[0] ),       /* has integer
	                                                                   subscript */
	                 ( (void)std::declval<S const &>( ).empty( ) ), /* has empty(
	                                                                   ) member */
	                 ( (void)std::declval<S const &>( ).data( ) ),  /* has data( )
	                                                                   member */
	                 ( (void)std::declval<S const &>( ).size( ) )   /* has size( )
	                                                                   member */
	                 )>> = true;
} // namespace daw::traits_is_sv

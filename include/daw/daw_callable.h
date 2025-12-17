// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#if defined( DAW_HAS_CPP20_CONCEPTS )
#include <utility>
#endif
#include <type_traits>

namespace daw {
#if defined( DAW_HAS_CPP20_CONCEPTS )
	template<typename R, typename Fn, typename... Args>
	concept is_callable_r_v = requires( Fn && fn, Args &&...args ) {
		{ std::forward<Fn>( fn )( std::forward<Args>( args )... ) }
		  ->std::convertible_to<R>;
	};

	template<typename Fn, typename... Args>
	concept is_callable_v = requires( Fn && fn, Args &&...args ) {
		std::forward<Fn>( fn )( std::forward<Args>( args )... );
	};

	template<typename R, typename Fn, typename... Args>
	concept is_nothrow_r_v = requires( Fn && fn, Args &&...args ) {
		{
			std::forward<Fn>( fn )( std::forward<Args>( args )... )
		} noexcept -> std::convertible_to<R>;
	};

	template<typename Fn, typename... Args>
	concept is_nothrow_callable_v = requires( Fn fn, Args... args ) {
		{ std::forward<Fn>( fn )( std::forward<Args>( args )... ) } noexcept;
	};
#else
	template<typename Fn, typename... Args>
	inline constexpr bool is_callable_v = std::is_invocable_v<Fn, Args...>;

	template<typename R, typename Fn, typename... Args>
	inline constexpr bool is_callable_r_v = std::is_invocable_r_v<R, Fn, Args...>;

	template<typename Fn, typename... Args>
	inline constexpr bool is_nothrow_callable_v =
	  std::is_nothrow_invocable_v<Fn, Args...>;

	template<typename R, typename Fn, typename... Args>
	inline constexpr bool is_nothrow_callable_r_v =
	  std::is_nothrow_invocable_r_v<R, Fn, Args...>;
#endif
} // namespace daw

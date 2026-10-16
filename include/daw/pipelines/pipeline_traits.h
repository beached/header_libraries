// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_traits.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace daw::pipelines::traits {

	template<typename Fn, typename... Params>
	concept PipelineInvocable = true;

	template<typename Fn, typename... Params>
	concept PipelineInvocable2 =
	  std::invocable<Fn, Params...> and
	  not std::same_as<void, std::invoke_result_t<Fn, Params...>>;

	template<typename Fn, typename Arg, std::size_t... Is>
	consteval bool is_applicable_impl( std::index_sequence<Is...> ) {
		return std::invocable<
		  Fn, decltype( std::get<Is>( std::declval<Arg>( ) ) )...>;
	}

	template<typename Fn, typename Arg>
	inline constexpr bool is_applicable_v = [] {
		using tuple_t = std::remove_cvref_t<Arg>;
		if constexpr( requires { typename std::tuple_size<tuple_t>::type; } ) {
			return is_applicable_impl<Fn, Arg>(
			  std::make_index_sequence<std::tuple_size_v<tuple_t>>{ } );
		} else {
			return false;
		}
	}( );

	template<std::size_t, typename T>
	using ignore_num = T;

	template<typename Fn, typename Arg, typename IndexSequence>
	struct apply_result_impl;

	template<typename Fn, typename Arg, std::size_t... Is>
	struct apply_result_impl<Fn, Arg, std::index_sequence<Is...>> {
		using type = std::invoke_result_t<
		  Fn, decltype( std::get<Is>( std::declval<Arg>( ) ) )...>;
	};

	template<typename Fn, typename Arg>
	struct apply_result
	  : apply_result_impl<
	      Fn,
	      Arg,
	      std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Arg>>>> {};

	template<typename Fn, typename Arg>
	using apply_result_t = typename apply_result<Fn, Arg>::type;

	template<typename Fn, typename Arg>
	concept NoVoidApplyResults = not std::same_as<apply_result_t<Fn, Arg>, void>;

	template<typename Fn, typename Arg>
	concept NoVoidResults = not std::same_as<std::invoke_result_t<Fn, Arg>, void>;
} // namespace daw::pipelines::traits

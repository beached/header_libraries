// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <tuple>
#include <type_traits>

namespace daw::pipelines::traits {

	template<typename Fn, typename... Params>
	concept PipelineInvocable = true;

	template<typename Fn, typename... Params>
	concept PipelineInvocable2 =
	  std::is_invocable_v<Fn, Params...> and
	  not std::same_as<void, std::invoke_result_t<Fn, Params...>>;

	template<typename Fn, typename Arg>
	inline constexpr bool is_applicable_v = false;

	template<typename Fn, typename... Ts>
	inline constexpr bool is_applicable_v<Fn, std::tuple<Ts...>> =
	  std::is_invocable_v<Fn, Ts...>;

	template<typename Fn, typename Arg>
	struct apply_result {};

	template<typename Fn, typename... Ts>
	struct apply_result<Fn, std::tuple<Ts...>> {
		using type = std::invoke_result_t<Fn, Ts...>;
	};

	template<typename Fn, typename Arg>
	using apply_result_t = typename apply_result<Fn, Arg>::type;

	template<typename Fn, typename Arg>
	concept NoVoidApplyResults = not std::same_as<apply_result_t<Fn, Arg>, void>;

	template<typename Fn, typename Arg>
	concept NoVoidResults = not std::same_as<std::invoke_result_t<Fn, Arg>, void>;
} // namespace daw::pipelines::traits

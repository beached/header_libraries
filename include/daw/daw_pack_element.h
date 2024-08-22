// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_undefined.h"
#include "traits/daw_traits_nth_element.h"

#include <cstddef>

namespace daw {
	template<std::size_t Idx, typename Pack>
	struct pack_element;

	template<std::size_t Idx, template<class...> class Pack, typename... Ts>
	struct pack_element<Idx, Pack<Ts...>> {
		using type = daw::traits::nth_type<Idx, Ts...>;
	};

	template<std::size_t Idx, typename Pack>
	using pack_element_t = typename pack_element<Idx, Pack>::type;

	template<typename Pack>
	inline constexpr auto pack_size_v = deleted<Pack>;

	template<template<typename...> typename Pack, typename... Ts>
	inline constexpr std::size_t pack_size_v<Pack<Ts...>> = sizeof...( Ts );

	template<typename Pack>
	using pack_size = std::integral_constant<std::size_t, pack_size_v<Pack>>;
} // namespace daw

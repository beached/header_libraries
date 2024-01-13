// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_traits_pack_list.h"

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>

namespace daw::traits {
#if DAW_HAS_BUILTIN( __type_pack_element )
	template<std::size_t I, typename... Ts>
	using nth_type = __type_pack_element<I, Ts...>;
#else
	namespace nth_type_impl {
		template<std::size_t I, typename T>
		struct nth_type_leaf {};

		template<typename TPack, typename IPack>
		struct nth_type_base;

		template<typename... Ts, std::size_t... Is>
		struct nth_type_base<std::index_sequence<Is...>, daw::pack_list<Ts...>>
		  : nth_type_leaf<Is, Ts>... {};

		template<typename... Ts>
		using nth_type_impl =
		  nth_type_base<std::index_sequence_for<Ts...>, daw::pack_list<Ts...>>;

		template<std::size_t I, typename T>
		auto find_leaf_type( nth_type_leaf<I, T> const & ) -> identity<T>;
	} // namespace nth_type_impl

	template<std::size_t Idx, typename... Ts>
	using nth_type = typename decltype( nth_type_impl::find_leaf_type<Idx>(
	  std::declval<nth_type_impl::nth_type_impl<Ts...>>( ) ) )::type;
#endif
	template<std::size_t I, typename... Ts>
	using nth_element = nth_type<I, Ts...>;
} // namespace daw::traits

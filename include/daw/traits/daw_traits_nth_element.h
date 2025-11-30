// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_cpp_feature_check.h"

#include <cstddef>


// clang-format off
#if not defined( DAW_CPP26_PACK_INDEXING ) and not DAW_HAS_BUILTIN( __type_pack_element )
// clang-format on
#include "daw/daw_nth_pack_element.h"
#include "daw_traits_identity.h"
#endif

namespace daw::traits {
#if defined( DAW_CPP26_PACK_INDEXING )
	// clang-format off
	template<std::size_t Idx, typename...Pack>
  using nth_element = Pack...[Idx];
	// clang-format on
#elif DAW_HAS_BUILTIN( __type_pack_element )
	template<std::size_t I, typename... Ts>
	using nth_element = __type_pack_element<I, Ts...>;
#else
	template<std::size_t Idx, typename... Ts>
	using nth_element = typename decltype( nth_pack_element<Idx>(
		daw::traits::identity<Ts>{}... ) )::type;
#endif
	template<std::size_t Idx, typename... Ts>
	using nth_type = nth_element<Idx, Ts...>;
} // namespace daw::traits
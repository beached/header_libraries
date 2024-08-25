// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"
#include "daw_ignored.h"
#include "daw_move.h"

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>

namespace daw {
	namespace nth_pack_element_impl {
		template<std::size_t... Is, typename... Ts>
		DAW_ATTRIB_INLINE constexpr decltype( auto )
		nth_pack_element_impl( std::index_sequence<Is...>, Ts &&...values ) {
			return []( ignore_constants_t<Is>...,
			           auto &&nth,
			           auto &&... ) -> decltype( nth ) && {
				return DAW_FWD( nth );
			}( DAW_FWD( values )... );
		}
	} // namespace nth_pack_element_impl

	template<std::size_t Idx, typename... Ts>
	constexpr auto nth_pack_element( Ts &&...values ) {
		static_assert( Idx < sizeof...( Ts ), "Index out of range" );
		return nth_pack_element_impl::nth_pack_element_impl(
		  std::make_index_sequence<Idx>{ }, DAW_FWD( values )... );
	}

	template<std::size_t Idx, typename... Ts>
	constexpr decltype( auto ) forward_nth_pack_element( Ts &&...values ) {
		static_assert( Idx < sizeof...( Ts ), "Index out of range" );
		return nth_pack_element_impl::nth_pack_element_impl(
		  std::make_index_sequence<Idx>{ }, DAW_FWD( values )... );
	}
} // namespace daw

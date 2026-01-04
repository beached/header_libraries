// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cxmath.h"

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_ensure.h>

#include <bitset>
#include <cassert>
#include <ranges>

namespace daw {
	namespace create_bs_impl {
		template<std::size_t N>
		DAW_CPP23_CX_BITSET_CX auto
		create_bitset_from_set_positions( auto const &r ) {
			auto result = std::bitset<N>{ };
			using value_t = std::ranges::range_value_t<decltype( r )>;
			for( auto const &position : r ) {
				assert( ( std::is_unsigned_v<value_t> or position > 0 ) );
				assert( static_cast<std::size_t>( position ) < N );
				result.set( static_cast<std::size_t>( position ) );
			}
			return result;
		}

	} // namespace create_bs_impl

	template<std::size_t N, typename R>
	requires( std::ranges::range<R>
	            and std::is_integral_v<std::ranges::range_value_t<R>> ) //
	  DAW_CPP23_CX_BITSET_CX auto create_bitset_from_set_positions( R const &r ) {
		return create_bs_impl::create_bitset_from_set_positions<N>( r );
	}

	template<std::size_t N, typename T>
	DAW_CPP23_CX_BITSET_CX auto
	create_bitset_from_set_positions( std::initializer_list<T> r ) {
		return create_bs_impl::create_bitset_from_set_positions<N>( r );
	}
} // namespace daw

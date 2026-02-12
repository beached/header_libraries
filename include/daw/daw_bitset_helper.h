// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_ensure.h"
#include "daw/traits/daw_traits_integer_type.h"

#include <bitset>
#include <ranges>

namespace daw {
	namespace create_bs_impl {
		template<std::size_t N>
		DAW_CPP23_CX_BITSET_FN auto
		create_bitset_from_set_positions( auto const &r ) {
			auto result = std::bitset<N>{ };
			for( auto const &position : r ) {
#ifndef NDEBUG
				using value_t = daw::traits::integral_type_t<
				  std::ranges::range_value_t<decltype( r )>>;
				daw_ensure( ( std::is_unsigned_v<value_t> or
				              static_cast<value_t>( position ) >= 0 ) );
				daw_ensure( static_cast<std::size_t>( position ) < N );
#endif
				result.set( static_cast<std::size_t>( position ) );
			}
			return result;
		}
	} // namespace create_bs_impl

	template<std::size_t N, typename R>
	requires( std::ranges::range<R>
	            and daw::traits::IntegerEnum<std::ranges::range_value_t<R>> ) //
	  DAW_ATTRIB_FLATTEN DAW_CPP23_CX_BITSET_FN
	  auto create_bitset_from_set_positions( R const &r ) {
		return create_bs_impl::create_bitset_from_set_positions<N>( r );
	}

	template<std::size_t N, daw::traits::IntegerEnum T>
	DAW_ATTRIB_FLATTEN DAW_CPP23_CX_BITSET_FN auto
	create_bitset_from_set_positions( std::initializer_list<T> r ) {
		return create_bs_impl::create_bitset_from_set_positions<N>( r );
	}

	template<std::size_t N>
	DAW_ATTRIB_FLATTEN DAW_CPP23_CX_BITSET_FN auto
	create_bitset_from_set_positions( traits::IntegerEnum auto... positions ) {
		auto result = std::bitset<N>{ };
		( result.set( [&]( auto position ) {
#ifndef NDEBUG
			using value_t = daw::traits::integral_type_t<decltype( position )>;
			daw_ensure( ( std::is_unsigned_v<value_t> or
			              static_cast<value_t>( position ) >= 0 ) );
			daw_ensure( static_cast<std::size_t>( position ) < N );
#endif
			return static_cast<std::size_t>( position );
		}( positions ) ),
		  ... );
		return result;
	}
} // namespace daw

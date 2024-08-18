// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "pipeline_traits.h"

#include <functional>

namespace daw::pipelines {
	template<typename Fn>
	[[nodiscard]] constexpr auto ForEach( Fn &&fn ) {
		return [=]<ForwardRange R>( R &&r ) {
			static_assert( std::is_invocable_v<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with invoke and passed value" );
			for( auto &&v : r ) {
				(void)std::invoke( fn, v );
			}
			return DAW_FWD( r );
		};
	}

	template<typename Fn>
	[[nodiscard]] constexpr auto ForEachApply( Fn &&fn ) {
		return [=]<ForwardRange R>( R &&r ) {
			static_assert( traits::is_applicable_v<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with apply and passed value" );
			for( auto &&v : r ) {
				(void)std::apply( fn, v );
			}
			return DAW_FWD( r );
		};
	}
} // namespace daw::pipelines

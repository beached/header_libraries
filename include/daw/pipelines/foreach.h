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

namespace daw::pipelines::pimpl {
	template<typename Fn>
	struct ForEach_t {
		Fn fn;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			static_assert( std::is_invocable_v<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with invoke and passed value" );
			for( auto &&v : r ) {
				(void)std::invoke( fn, v );
			}
			return DAW_FWD( r );
		}
	};
	template<typename Fn>
	ForEach_t( Fn ) -> ForEach_t<Fn>;

	template<typename Fn>
	struct ForEachApply_t {
		Fn fn;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			static_assert( traits::is_applicable_v<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with apply and passed value" );
			for( auto &&v : r ) {
				(void)std::apply( fn, v );
			}
			return DAW_FWD( r );
		}
	};
	template<typename Fn>
	ForEachApply_t( Fn ) -> ForEachApply_t<Fn>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto ForEach( auto &&fn ) {
		return pimpl::ForEach_t{ DAW_FWD( fn ) };
	}

	[[nodiscard]] constexpr auto ForEachApply( auto &&fn ) {
		return pimpl::ForEachApply_t{ DAW_FWD( fn ) };
	}
} // namespace daw::pipelines
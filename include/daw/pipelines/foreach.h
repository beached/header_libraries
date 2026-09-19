// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/range_base.h"

#include <functional>

namespace daw::pipelines::pimpl {
	template<typename Fn>
	struct ForEach_t {
		DAW_NO_UNIQUE_ADDRESS mutable Fn fn;

		template<Range R>
		[[nodiscard]] constexpr daw::remove_rvalue_ref_t<R>
		operator( )( R &&r ) const {
			static_assert( std::invocable<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with invoke and passed value" );
			for( auto &&v : DAW_FWD( r ) ) {
				(void)std::invoke( fn, DAW_FWD( v ) );
			}
			if constexpr( std::is_rvalue_reference_v<decltype( r )> ) {
				using result_t = std::remove_cvref_t<decltype( r )>;
				return result_t{ DAW_FWD( r ) };
			} else {
				return DAW_FWD( r );
			}
		}
	};
	template<typename Fn>
	ForEach_t( Fn ) -> ForEach_t<Fn>;

	template<typename Fn>
	struct ForEachApply_t {
		DAW_NO_UNIQUE_ADDRESS mutable Fn fn;

		template<Range R>
		[[nodiscard]] constexpr daw::remove_rvalue_ref_t<R>
		operator( )( R &&r ) const {
			static_assert( traits::is_applicable_v<Fn, range_reference_t<R>>,
			               "ForEach requires the function to be able to be called "
			               "with apply and passed value" );
			for( auto &&v : DAW_FWD( r ) ) {
				(void)std::apply( fn, DAW_FWD( v ) );
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

	[[nodiscard]] constexpr auto ForEachIndexed( auto &&fn ) {
		// Maybe used owned range
		return [fun = DAW_FWD( fn )]<RandomRange R>(
		         R &&r ) -> daw::remove_rvalue_ref_t<R> {
			auto const sz =
			  static_cast<std::size_t>( daw::pipelines::pimpl::ranges_distance(
			    std::begin( r ), std::end( r ) ) );
			for( std::size_t n = 0; n < sz; ++n ) {
				(void)std::invoke( fun, r[n] );
			}
			return DAW_FWD( r );
		};
	}
} // namespace daw::pipelines

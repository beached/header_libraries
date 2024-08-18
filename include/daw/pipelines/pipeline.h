// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_concepts.h"
#include "daw/daw_contiguous_view.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_string_view.h"
#include "pipeline_traits.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace daw::pipelines::impl {

	template<typename Fn>
	struct pipeline_t {
		Fn const &fn;

		template<typename R>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto operator( )( R &&r ) const {
			if constexpr( std::is_invocable_v<Fn> ) {
				return std::invoke( fn( ), DAW_FWD( r ) );
			} else {
				return std::invoke( fn, DAW_FWD( r ) );
			}
		}

		template<typename R, typename FNNext>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator( )( R &&r, FNNext const &fn_next, auto const &...fns ) const {
			if constexpr( std::is_invocable_v<Fn> ) {
				return pipeline_t<FNNext>{ fn_next }(
				  std::invoke( fn( ), DAW_FWD( r ) ), fns... );
			} else {
				return pipeline_t<FNNext>{ fn_next }( std::invoke( fn, DAW_FWD( r ) ),
				                                      fns... );
			}
		}

		template<typename R, typename FNNext>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator( )( R &&r, FNNext const &fn_next, auto const &...fns ) {
			if constexpr( std::is_invocable_v<Fn> ) {
				return pipeline_t<FNNext>{ fn_next }(
				  std::invoke( fn( ), DAW_FWD( r ) ), fns... );
			} else {
				return pipeline_t<FNNext>{ fn_next }( std::invoke( fn, DAW_FWD( r ) ),
				                                      fns... );
			}
		}
	};
	template<typename Fn>
	pipeline_t( Fn const & ) -> pipeline_t<Fn>;

	template<std::size_t N, typename Fn>
	[[nodiscard]] constexpr auto
	pipeline( char const ( &s )[N], Fn const &fn, auto const &...fns ) {
		return pipeline_t{ fn }( daw::string_view( s, N - 1 ), fns... );
	}

	template<typename T, std::size_t N, typename Fn>
	[[nodiscard]] constexpr auto
	pipeline( T ( &r )[N], Fn const &fn, auto const &...fns ) {
		return pipeline_t{ fn }( daw::contiguous_view( r, N ), fns... );
	}

	template<typename R, typename Fn>
	[[nodiscard]] constexpr auto
	pipeline( R &&r, Fn const &fn, auto const &...fns ) {
		return pipeline_t{ fn }( DAW_FWD( r ), fns... );
	}
} // namespace daw::pipelines::impl

namespace daw::pipelines {
	template<typename Fn>
	DAW_ATTRIB_FLATTEN constexpr auto pipeline( Fn &&fn, auto &&...fns ) {
		return [=]<typename R>( R &&r ) {
			return impl::pipeline( DAW_FWD( r ), fn, fns... );
		};
	}

	template<Range R, typename Fn>
	DAW_ATTRIB_FLATTEN constexpr auto pipeline( R &&r, Fn &&fn, auto &&...fns ) {
		return impl::pipeline( DAW_FWD( r ), DAW_FWD( fn ), DAW_FWD( fns )... );
	}
} // namespace daw::pipelines

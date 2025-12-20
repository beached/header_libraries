// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "daw/pipelines/range.h"

namespace daw::pipelines {
	template<typename Pred>
	constexpr auto DropWhile( Pred &&p ) {
		return [pred = DAW_FWD( p )]( auto &&r ) {
			auto f = std::begin( r );
			auto l = std::end( r );
			while( f != l and pred( *f ) ) {
				++f;
			}
			return range_t{ std::move( f ), std::move( l ) };
		};
	}

	template<typename Pred>
	constexpr auto DropUntil( Pred &&p ) {
		return [pred = DAW_FWD( p )]( auto &&r ) {
			auto f = std::begin( r );
			auto l = std::end( r );
			while( f != l and not pred( *f ) ) {
				++f;
			}
			return range_t{ std::move( f ), std::move( l ) };
		};
	}
} // namespace daw::pipelines
// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "range.h"
#include "sized_iterator.h"

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace daw::pipelines::pimpl {
	struct Skip_t {
		std::size_t how_many;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			using iter_t = iterator_t<R>;
			auto first = std::begin( r );
			auto last = std::end( r );

			if constexpr( ForwardIterator<iter_t> ) {
				auto const range_size = std::distance( first, last );
				auto const skip =
				  std::min( { static_cast<std::ptrdiff_t>( how_many ), range_size } );
				first = std::next( first, skip );
			} else {
				// Input
				for( auto n = how_many; n > 0; --n ) {
					if( first == last ) {
						break;
					}
					++first;
				}
			}
			return range_t{ first, last };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto Skip( std::size_t how_many ) {
		return pimpl::Skip_t{ how_many };
	}
} // namespace daw::pipelines

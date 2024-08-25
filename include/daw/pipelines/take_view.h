// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_typeof.h"
#include "range.h"
#include "sized_iterator.h"
#include "skip.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>

namespace daw::pipelines::pimpl {
	struct Take_t {
		std::size_t how_many = 0;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			using iter_t = iterator_t<decltype( r )>;
			auto first = std::begin( r );
			auto last = std::end( r );

			if constexpr( ForwardIterator<iter_t> ) {
				auto const range_size = std::distance( first, last );
				auto take_size =
				  std::min( { range_size, static_cast<std::ptrdiff_t>( how_many ) } );
				return range_t{ sized_iterator<iter_t>{
				                  first, static_cast<std::size_t>( take_size ) },
				                sized_iterator<iter_t>{ last, 0 } };
			} else {
				return range_t{ sized_iterator<iter_t>{ first, how_many },
				                sized_iterator<iter_t>{ last, 0 } };
			}
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto Take( std::size_t how_many ) {
		return pimpl::Take_t{ how_many };
	}
} // namespace daw::pipelines

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_ensure.h"
#include "daw/daw_iterator_traits.h"

namespace daw::pipelines::pimpl {
	template<Iterator First>
	constexpr auto safe_move_next(
	  First first, Iterator auto const &last,
	  daw::iter_difference_t<First> n = daw::iter_difference_t<First>{ 1 } ) {
		using diff_t = daw::iter_difference_t<First>;
		daw_ensure( n >= diff_t{ 0 } );
		if constexpr( std::is_base_of_v<std::random_access_iterator_tag,
		                                iter_category_t<First>> ) {
			std::advance( first, std::min( { range_distance( first, last ), n } ) );
		} else {
			while( first != last and n > diff_t{ 0 } ) {
				++first;
				--n;
			}
		}
		return first;
	}
} // namespace daw::pipelines::pimpl

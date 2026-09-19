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
	constexpr auto safe_move_next( Iterator auto first, Iterator auto const &last,
	                               std::ptrdiff_t n = 1 ) {
		daw_ensure( n >= 0 );
		if constexpr( std::is_base_of_v<std::random_access_iterator_tag,
		                                iter_category_t<decltype( first )>> ) {
			std::advance( first,
			              std::min( { std::ranges::distance( first, last ), n } ) );
		} else {
			while( first != last and n > 0 ) {
				++first;
				--n;
			}
		}
		return first;
	}
} // namespace daw::pipelines::pimpl

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/filter.h"
#include "daw/pipelines/range.h"

#include <cstddef>

namespace daw::pipelines::pimpl {
	struct Every_t {
		std::size_t m_select_every;

		[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
			return filter_view{ std::begin( DAW_FWD( r ) ), std::end( DAW_FWD( r ) ),
			                    [select_every = m_select_every,
			                     n = m_select_every]( auto const & ) mutable {
				                    if( n >= select_every ) {
					                    n = 1;
					                    return true;
				                    }
				                    ++n;
				                    return false;
			                    } };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	/// Given a Range, output every Nth element
	[[nodiscard]] constexpr auto Every( std::size_t select_every ) {
		return pimpl::Every_t{ select_every };
	}
} // namespace daw::pipelines

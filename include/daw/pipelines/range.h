// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <iterator>

namespace daw::pipelines {
	template<Iterator First, Iterator Last>
	struct range_t {
		First first;
		Last last;

		[[nodiscard]] constexpr First begin( ) const {
			return first;
		}

		[[nodiscard]] constexpr Last end( ) const {
			return last;
		}
	};
	template<typename I, typename L>
	range_t( I, L ) -> range_t<I, L>;
} // namespace daw::pipelines

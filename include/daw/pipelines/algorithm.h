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
#include "range.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace daw::pipelines {
	template<typename R>
	concept Sortable = Range<R> and RandomIterator<iterator_t<R>>;

	namespace impl {
		struct Sort_t {
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr decltype( auto )
			operator( )( Sortable auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				std::sort( std::begin( r ), std::end( r ) );
				return DAW_FWD( r );
			}
		};
	} // namespace impl
	inline constexpr auto Sort = impl::Sort_t{ };

} // namespace daw::pipelines

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
#include "daw/daw_move.h"
#include "daw/iterator/daw_reverse_iterator.h"
#include "range.h"

#include <utility>

namespace daw::pipelines::pimpl {
	struct Reverse_t {
		template<Range R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			static_assert(
			  BidirectionalRange<R>,
			  "The range passed to Reverse must have Bidirectional Iterators" );
			auto first = daw::reverse_iterator( std::end( DAW_FWD( r ) ) );
			auto last = daw::reverse_iterator( std::begin( DAW_FWD( r ) ) );
			return range_t{ std::move( first ), std::move( last ) };
		}
	};
} // namespace daw::pipelines::pimpl
namespace daw::pipelines {
	inline constexpr auto Reverse = pimpl::Reverse_t{ };
}

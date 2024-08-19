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

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	namespace pipelines_impl {
		template<ForwardIterator It, typename Last>
		constexpr It
		safe_next( It first, Last const &last, std::size_t how_many = 1U ) {
			auto const max_how_many = std::distance( first, last );
			auto const h =
			  std::min( static_cast<std::ptrdiff_t>( how_many ), max_how_many );
			return std::next( last, h );
		}

		struct Take_t {
			std::size_t how_many_to_skip = 0;
			std::size_t how_many = 0;

			template<typename R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				static_assert( Range<R>, "Take requires a Range passed to it" );
				auto first =
				  how_many_to_skip == 0
				    ? std::begin( r )
				    : safe_next( std::begin( r ), std::end( r ), how_many_to_skip );
				auto last = safe_next( first, std::end( r ), how_many );
				return range_t{ first, last };
			}
		};
	} // namespace pipelines_impl

	[[nodiscard]] constexpr auto Take( std::size_t how_many ) {
		return pipelines_impl::Take_t{ 0, how_many };
	}

	[[nodiscard]] constexpr auto Take( std::size_t how_many_to_skip,
	                                   std::size_t how_many ) {
		return pipelines_impl::Take_t{ how_many_to_skip, how_many };
	}
} // namespace daw::pipelines

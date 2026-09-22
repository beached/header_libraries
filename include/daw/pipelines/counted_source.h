// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"

#include <concepts>
#include <cstddef>
#include <limits>

namespace daw::pipelines::pimpl {
	/// A random access iterator whose distance to the end is known in O(1).  A
	/// view over such a source can count the elements it has left instead of
	/// comparing against the end on every step.  That lets the compiler see a
	/// counted loop, and vectorize it.
	template<typename First, typename Last>
	concept counted_source =
	  RandomIterator<First> and requires( First f, Last l ) {
		  { l - f } -> std::convertible_to<std::ptrdiff_t>;
	  };

	/// The number of elements from first to last.  A distance that does not fit
	/// in std::ptrdiff_t, e.g. an iota_view<std::size_t> that runs to the largest
	/// value, is reported as unbounded
	template<typename First, typename Last>
	[[nodiscard]] constexpr std::ptrdiff_t counted_length( First const &first,
	                                                       Last const &last ) {
		auto const distance = static_cast<std::ptrdiff_t>( last - first );
		return distance < 0 ? std::numeric_limits<std::ptrdiff_t>::max( )
		                    : distance;
	}
} // namespace daw::pipelines::pimpl

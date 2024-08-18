// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "daw_algorithm_accumulate.h"

#include <daw/stdinc/iterator_traits.h>

namespace daw::algorithm {
	template<typename InputIterator,
	         typename OutputIterator,
	         typename BinaryOperator = accum_impl::plus>
	constexpr OutputIterator
	partial_sum( InputIterator first,
	             InputIterator last,
	             OutputIterator first_out,
	             BinaryOperator op = BinaryOperator{ } ) {

		if( first == last ) {
			return first_out;
		}

		using value_type = typename std::iterator_traits<InputIterator>::value_type;

		value_type sum = *first;
		*first_out = sum;
		++first_out;
		++first;
		while( first != last ) {
			sum = op( std::move( sum ), *first );
			++first;

			*first_out = sum;
			++first_out;
		}
		return first_out;
	}
} // namespace daw::algorithm

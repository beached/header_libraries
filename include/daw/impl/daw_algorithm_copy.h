// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_compiler_fixups.h"

namespace daw::algorithm {
	/// @brief Copy input range [first, last) to output range [first_out,
	/// first_out + std::distance( first, last ))
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam LastType type of Iterator marking end of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param last end of input range
	/// @param first_out first item in output range
	/// @return end of output range written to
	template<typename InputIterator, typename LastType, typename OutputIterator>
	constexpr OutputIterator
	copy( InputIterator first, LastType last,
	      OutputIterator first_out ) noexcept( noexcept( *first_out = *first ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );
		DAW_UNSAFE_BUFFER_FUNC_START
		while( first != last ) {
			*first_out = *first;
			++first;
			++first_out;
		}
		return first_out;
		DAW_UNSAFE_BUFFER_FUNC_STOP
	}

} // namespace daw::algorithm

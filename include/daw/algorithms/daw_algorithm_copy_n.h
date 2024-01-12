// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_compiler_fixups.h"
#include "daw/daw_traits_concepts.h"

namespace daw::algorithm {
	/// @brief Copy input range [first, last) to output range [first_out,
	/// first_out + count)
	/// @tparam InputIterator type of Iterator of input range
	/// @tparam OutputIterator type of iterator for output range
	/// @param first start of input range
	/// @param first_out first item in output range
	/// @param count number of items to copy
	/// @return end of output range written to
	template<typename InputIterator, typename OutputIterator>
	constexpr auto
	copy_n( InputIterator first, OutputIterator first_out,
	        size_t count ) noexcept( noexcept( *first_out = *first ) ) {

		traits::is_input_iterator_test<InputIterator>( );
		traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );

		while( count-- > 0 ) {
			DAW_UNSAFE_BUFFER_FUNC_START
			*first_out = *first;
			++first;
			++first_out;
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}
		struct result_t {
			InputIterator input;
			OutputIterator output;
		};
		return result_t{ first, first_out };
	}
} // namespace daw::algorithm

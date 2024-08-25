// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw_common_mixins.h"
#include "daw_heap_value.h"

#include <vector>

namespace daw {
	template<typename T>
	class poly_vector_t
	  : public daw::mixins::VectorLikeProxy<poly_vector_t<T>,
	                                        std::vector<daw::heap_value<T>>> {
		std::vector<daw::heap_value<T>> m_values;

	public:
		std::vector<daw::heap_value<T>> &container( ) {
			return m_values;
		}

		std::vector<daw::heap_value<T>> const &container( ) const {
			return m_values;
		}
	}; // poly_vector_t
} // namespace daw

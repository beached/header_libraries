// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/traits/daw_traits_concepts.h"

namespace daw {
	template<typename Iterator>
	constexpr std::move_iterator<Iterator> make_move_iterator( Iterator &&i ) {
		traits::is_iterator_test<Iterator>( );

		return std::move_iterator<Iterator>( DAW_FWD( i ) );
	}
} // namespace daw

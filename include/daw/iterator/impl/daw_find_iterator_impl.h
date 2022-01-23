// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../../daw_move.h"

namespace daw::find_iterator_impl {
	constexpr daw::traits::identity<std::forward_iterator_tag>
	test_iterator_category( std::forward_iterator_tag const & ) {
		return { };
	}

	constexpr daw::traits::identity<std::input_iterator_tag>
	test_iterator_category( std::input_iterator_tag const & ) {
		return { };
	}

	template<typename Filter, bool>
	struct FilterProxy : protected Filter {
		constexpr FilterProxy( Filter filt )
		  : Filter{ filt } {}

	protected:
		constexpr Filter const &filter( ) const {
			return static_cast<Filter const &>( *this );
		}

		constexpr Filter &filter( ) {
			return static_cast<Filter &>( *this );
		}
	};

	template<typename Filter>
	struct FilterProxy<Filter, false> {
	protected:
		Filter m_filter;

		constexpr auto filter( ) const {
			return
			  [&]( auto &&value ) -> bool { return m_filter( DAW_FWD( value ) ); };
		}
	};
} // namespace daw::find_iterator_impl

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_remove_cvref.h"

#include <cstddef>
#include <daw/stdinc/tuple_traits.h>

namespace daw::pipelines {
	template<typename First, typename Last = First>
	struct range_base_t {
		using i_am_a_daw_pipelines_range_base_t = void;
		using iterator_first_t = First;
		using iterator_last_t = Last;

		range_base_t( ) = default;
	};

	template<typename R>
	concept RangeBase = requires {
		typename daw::remove_cvref_t<R>::i_am_a_daw_pipelines_range_base_t;
	};
} // namespace daw::pipelines

namespace std {
	template<daw::pipelines::RangeBase R>
	inline constexpr std::size_t tuple_size_v<R> = 2;

	template<daw::pipelines::RangeBase R>
	struct tuple_size<R> {
		static constexpr std::size_t value = 2;
	};

	template<std::size_t I, daw::pipelines::RangeBase R>
	struct tuple_element<I, R> {
		using type = typename R::iterator_first_t;
	};

	template<daw::pipelines::RangeBase R>
	struct tuple_element<1, R> {
		using type = typename R::iterator_last_t;
	};

	template<std::size_t I>
	constexpr auto get( daw::pipelines::RangeBase auto &&r )
	  requires( I == 0 or I == 1 ) {
		if constexpr( I == 0 ) {
			return DAW_FWD( r ).begin( );
		} else {
			return DAW_FWD( r ).end( );
		}
	}
} // namespace std

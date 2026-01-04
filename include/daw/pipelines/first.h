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

#include <functional>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>

namespace daw::pipelines {
	constexpr auto First( ) {
		return []( Range auto &&r ) DAW_CPP23_STATIC_CALL_OP {
			auto f = std::begin( r );
			auto l = std::end( r );
			using ref_t = daw::iter_reference_t<decltype( f )>;
#if defined( DAW_HAS_CPP26_OPTIONAL_REFS )
			using result_t =
			  std::conditional_t<std::is_lvalue_reference<ref_t>::value,
			                     ref_t,
			                     std::remove_cvref_t<ref_t>>;
#else
			using result_t = std::conditional_t<
			  std::is_lvalue_reference<ref_t>::value,
			  std::reference_wrapper<std::remove_reference_t<ref_t>>,
			  std::remove_cvref_t<ref_t>>;
#endif
			if( f == l ) {
				return std::optional<result_t>( );
			}
			return std::optional<result_t>( std::in_place, *f );
		};
	}
} // namespace daw::pipelines

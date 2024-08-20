// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"

#include <type_traits>

namespace daw::pipelines {
	namespace pimple {
		template<auto... haystack>
		struct is_one_of_t {
			explicit is_one_of_t( ) = default;

			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( auto const &needle ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return ( ( needle == haystack ) or ... );
			}
		};

		template<typename Fn>
		struct not_fn_t {
			using func_t = daw::remove_cvref_t<Fn>;
			[[no_unique_address]] func_t fn;

			[[nodiscard]] constexpr auto operator( )( auto &&value ) const {
				return not fn( DAW_FWD( value ) );
			}
		};
		template<typename F>
		not_fn_t( F ) -> not_fn_t<F>;
	} // namespace pimple

	template<auto... haystack>
	inline constexpr auto IsOneOf = pimple::is_one_of_t<haystack...>{ };

	[[nodiscard]] constexpr auto Not( auto &&fn ) {
		return pimple::not_fn_t{ DAW_FWD( fn ) };
	};
} // namespace daw::pipelines

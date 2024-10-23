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
	namespace pimpl {
		template<auto... haystack>
		struct is_one_of_t {
			explicit is_one_of_t( ) = default;

			[[nodiscard]] DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( auto const &needle ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return ( ( needle == haystack ) or ... );
			}
		};

		template<typename Fn>
		struct not_fn_t {
			DAW_NO_UNIQUE_ADDRESS mutable Fn m_func{ };

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
			operator( )( auto &&value ) const {
				return not m_func( DAW_FWD( value ) );
			}
		};

		template<typename F>
		not_fn_t( F ) -> not_fn_t<F>;
	} // namespace pimpl

	template<auto... haystack>
	inline constexpr auto IsOneOf = pimpl::is_one_of_t<haystack...>{ };

	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto Not( auto &&fn ) {
		return pimpl::not_fn_t{ DAW_FWD( fn ) };
	};
} // namespace daw::pipelines

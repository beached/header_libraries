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
#include "iota.h"
#include "zip.h"

#include <cstddef>
#include <limits>

namespace daw::pipelines::pimpl {
	struct Enumerate_t {
		template<typename R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			static_assert( Range<R> );
			return zip_view<iota_view<std::size_t>, daw::remove_cvref_t<R>>(
			  iota_view<std::size_t>( 0, std::numeric_limits<std::size_t>::max( ) ),
			  DAW_FWD( r ) );
		}
	};

	struct EnumerateApply_t {
		template<typename R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if constexpr( RandomRange<R> ) {
				auto const sz = static_cast<std::size_t>(
				  std::distance( std::begin( r ), std::end( r ) ) );
				return ZipMore( iota_view<std::size_t>( 0, sz ), DAW_FWD( r ) );
			} else {
				return ZipMore(
				  iota_view<std::size_t>( 0, std::numeric_limits<std::size_t>::max( ) ),
				  DAW_FWD( r ) );
			}
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	inline constexpr auto Enumerate = pimpl::Enumerate_t{ };
	inline constexpr auto EnumerateApply = pimpl::EnumerateApply_t{ };
} // namespace daw::pipelines

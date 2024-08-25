// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "iota_view.h"
#include "map_view.h"

#include <functional>

namespace daw::pipelines::pimpl {
	template<typename Fn>
	struct GenerateN_t {
		DAW_NO_UNIQUE_ADDRESS mutable Fn m_func;

		[[nodiscard]] constexpr auto operator( )( std::size_t ) const {
			return std::invoke( m_func );
		}
	};
	template<typename Fn>
	GenerateN_t( Fn ) -> GenerateN_t<Fn>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto generate_n( std::size_t how_many, auto &&fn ) {
		return Map( pimpl::GenerateN_t{ DAW_FWD( fn ) } )( iota_view{ how_many } );
	}
} // namespace daw::pipelines

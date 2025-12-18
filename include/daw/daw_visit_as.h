// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_bit_cast.h>
#include <daw/daw_move.h>

#include <functional>

namespace daw {
	template<typename VisitedT, typename OrigT, typename Visitor>
#if defined( DAW_CX_BIT_CAST )
	constexpr
#endif
	  auto
	  visit_as( OrigT &value, Visitor &&visitor ) {
		auto tmp = daw::bit_cast<VisitedT>( value );
		if constexpr( std::is_void_v<std::invoke_result_t<Visitor, VisitedT &>> ) {
			std::invoke( DAW_FWD( visitor ), tmp );
			value = daw::bit_cast<OrigT>( tmp );
		} else {
			auto result = std::invoke( DAW_FWD( visitor ), tmp );
			value = daw::bit_cast<OrigT>( tmp );
			return result;
		}
	}

	template<typename VisitedT, typename OrigT, typename Visitor>
#if defined( DAW_CX_BIT_CAST )
	constexpr
#endif
	  auto
	  visit_as( OrigT const &value, Visitor &&visitor ) {
		auto tmp = daw::bit_cast<VisitedT>( value );
		return std::invoke( DAW_FWD( visitor ), tmp );
	}
} // namespace daw

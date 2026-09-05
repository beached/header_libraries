// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if not defined( NDEBUG ) and defined( DAW_FWD_CHECKER )
#include <type_traits>

namespace daw::forward_impl {
	template<typename T>
	struct forward_helper {
		static_assert( std::is_reference_v<T>,
		               "Forward can only forward references" );
		using type = T;
	};
} // namespace daw::forward_impl
#define DAW_FWD( ... )                                      \
	static_cast<typename ::daw::forward_impl::forward_helper< \
	  decltype( __VA_ARGS__ )>::type>( __VA_ARGS__ )

#define DAW_FWD2( ... ) static_cast<decltype( __VA_ARGS__ )>( __VA_ARGS__ )
#else
#ifndef DAW_FWD
#define DAW_FWD( ... ) static_cast<decltype( __VA_ARGS__ )>( __VA_ARGS__ )
#define DAW_FWD2( ... ) static_cast<decltype( __VA_ARGS__ )>( __VA_ARGS__ )
#endif
#endif
// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "traits/daw_traits_is_same.h"

#include <cstdlib>

namespace daw {
	template<typename T>
	struct undefined_t;

	namespace impl {
		template<typename T>
		struct undefind_fn_t {
			[[deprecated( "Use deleted<T...>" )]]
			auto operator( )( ) const {
				if constexpr( not daw::is_same_v<T, undefined_t<T>> ) {
					std::abort( );
				} else {
					return static_cast<T *>( nullptr );
				}
			}
		};
	} // namespace impl
	template<typename T>
	[[deprecated( "Use deleted<T...>" )]]
	inline constexpr auto undefined_v = impl::undefind_fn_t<T>{ };

	template<typename...>
	void deleted( ) = delete;
} // namespace daw

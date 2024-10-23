// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <algorithm>
#include <cstddef>

namespace daw {
	namespace impl {
		template<std::size_t Len, std::size_t Align>
		struct aligned_storage {
			union type {
				std::max_align_t align;
				unsigned char data[( Len + Align - 1 ) / Align * Align];
			};
		};
	} // namespace impl

	template<std::size_t Size, std::size_t Align = alignof( std::max_align_t )>
	using aligned_storage_t = typename impl::aligned_storage<Size, Align>::type;

	template<typename T, typename... Ts>
	using aligned_storage_for_t =
	  typename impl::aligned_storage<std::max( { sizeof( T ), sizeof( Ts )... } ),
	                                 std::max( { alignof( T ),
	                                             alignof( Ts )... } )>::type;
} // namespace daw

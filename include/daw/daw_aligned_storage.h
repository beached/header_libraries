// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <cstddef>

namespace daw {
	template <std::size_t Len, std::size_t Align = alignof( std::max_align_t )>
	struct aligned_storage {
		union type {
			std::max_align_t align;
			unsigned char data[(Len + Align - 1) / Align * Align];
		};
	};

	template<std::size_t Size, std::size_t Align = alignof( std::max_align_t )>
	using aligned_storage_t = typename aligned_storage<Size, Align>::type;
} // namespace daw

// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <cstddef>

namespace daw {
	namespace size_literals {
		constexpr size_t operator"" _KB( unsigned long long val ) noexcept {
			return val * 1024ull;
		}

		constexpr size_t operator"" _MB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _GB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _TB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull * 1024ull;
		}

		constexpr size_t operator"" _PB( unsigned long long val ) noexcept {
			return val * 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;
		}
	} // namespace size_literals
} // namespace daw

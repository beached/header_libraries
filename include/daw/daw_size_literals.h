// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>

namespace daw::size_literals {
	[[nodiscard]] constexpr size_t
	operator""_KB( unsigned long long val ) noexcept {
		return val * 1024ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_MB( unsigned long long val ) noexcept {
		return val * 1024ull * 1024ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_GB( unsigned long long val ) noexcept {
		return val * 1024ull * 1024ull * 1024ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_TB( unsigned long long val ) noexcept {
		return val * 1024ull * 1024ull * 1024ull * 1024ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_PB( unsigned long long val ) noexcept {
		return val * 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_KiB( unsigned long long val ) noexcept {
		return val * 1000ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_MiB( unsigned long long val ) noexcept {
		return val * 1000ull * 1000ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_GiB( unsigned long long val ) noexcept {
		return val * 1000ull * 1000ull * 1000ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_TiB( unsigned long long val ) noexcept {
		return val * 1000ull * 1000ull * 1000ull * 1000ull;
	}

	[[nodiscard]] constexpr size_t
	operator""_PiB( unsigned long long val ) noexcept {
		return val * 1000ull * 1000ull * 1000ull * 1000ull * 1000ull;
	}
} // namespace daw::size_literals

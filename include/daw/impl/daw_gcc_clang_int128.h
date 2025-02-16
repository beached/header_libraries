// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/impl/daw_int128_check.h"
#include "daw/impl/daw_numeric_limits.h"

#include <climits>
#include <limits>

#if defined( DAW_HAS_INT128 ) and not defined( DAW_HAS_MSVC )
#if defined( DAW_HAS_GCC_LIKE )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
namespace daw {
	using int128_t = __int128;
	using uint128_t = __uint128_t;
	// numeric_limits cannot be relied on here.
	template<>
	struct numeric_limits<__int128> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		// Cannot reasonably guess as it's imp defined for signed
		// static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( __int128 ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonably define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr __int128( min )( ) noexcept {
			return static_cast<__int128>(
			  static_cast<__uint128_t>( 0x8000'0000'0000'0000ULL ) << 64U );
		}

		static constexpr __int128 lowest( ) noexcept {
			return static_cast<__int128>(
			  static_cast<__uint128_t>( 0x8000'0000'0000'0000ULL ) << 64U );
		}

		static constexpr __int128( max )( ) noexcept {
			return static_cast<__int128>(
			  ( static_cast<__uint128_t>( 0x7FFF'FFFF'FFFF'FFFFULL ) << 64U ) |
			  ( static_cast<__uint128_t>( 0xFFFF'FFFF'FFFF'FFFFULL ) ) );
		}

		static constexpr __int128 epsilon( ) noexcept {
			return 0;
		}

		static constexpr __int128 round_error( ) noexcept {
			return 0;
		}

		static constexpr __int128 infinity( ) noexcept {
			return 0;
		}

		static constexpr __int128 quiet_NaN( ) noexcept {
			return 0;
		}

		static constexpr __int128 signalling_NaN( ) noexcept {
			return 0;
		}

		static constexpr __int128 denorm_min( ) noexcept {
			return 0;
		}
	};

	template<>
	struct numeric_limits<__uint128_t> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( __uint128_t ) * CHAR_BIT - is_signed );
		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonibly define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr __uint128_t( min )( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t lowest( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t( max )( ) noexcept {
			return static_cast<__uint128_t>(
			  ( static_cast<__uint128_t>( 0xFFFF'FFFF'FFFF'FFFFULL ) << 64U ) |
			  ( static_cast<__uint128_t>( 0xFFFF'FFFF'FFFF'FFFFULL ) ) );
		}

		static constexpr __uint128_t epsilon( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t round_error( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t infinity( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t quiet_NaN( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t signalling_NaN( ) noexcept {
			return 0;
		}

		static constexpr __uint128_t denorm_min( ) noexcept {
			return 0;
		}
	};
} // namespace daw

#if defined( DAW_HAS_GCC_LIKE )
#pragma GCC diagnostic pop
#endif
#endif
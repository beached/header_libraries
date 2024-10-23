// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
//

#pragma once

#include "ciso646.h"
#include "daw_cpp_feature_check.h"
#include "traits/daw_traits_conditional.h"

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

#if defined( DAW_JSON_NO_INT128 )
#if defined( DAW_HAS_INT128 )
#undef DAW_HAS_INT128
#endif
#elif defined( DAW_HAS_INT128 )
#elif defined( __SIZEOF_INT128__ )
#if ( defined( __clang__ ) and not defined( _WIN32 ) ) or    \
  ( defined( __CUDACC__ ) and __CUDACC_VER_MAJOR__ >= 9 ) or \
  ( defined( __GNUC__ ) and not defined( __clang__ ) and     \
    !defined( __CUDACC__ ) )
#define DAW_HAS_INT128
#elif defined( __CUDACC__ )
// __CUDACC_VER__ is a full version number before CUDA 9, and is defined to a
// string explaining that it has been removed starting with CUDA 9. We use
// nested #ifs because there is no short-circuiting in the preprocessor.
// NOTE: `__CUDACC__` could be undefined while `__CUDACC_VER__` is defined.
#if __CUDACC_VER__ >= 70000
#define DAW_HAS_INT128
#endif // __CUDACC_VER__ >= 70000
#endif // defined(__CUDACC__)
#endif

namespace daw {
	template<typename T>
	struct numeric_limits : std::numeric_limits<T> {};

#if defined( DAW_HAS_INT128 )
#if defined( DAW_HAS_GCC_LIKE )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

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
#if defined( DAW_HAS_GCC_LIKE )
#pragma GCC diagnostic pop
#endif
#endif

	template<typename T>
	using is_integral = std::bool_constant<daw::numeric_limits<T>::is_integer>;

	template<typename T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	static_assert( is_integral_v<int> );
	static_assert( not is_integral_v<float> );

	namespace arith_traits_details {
		template<typename T>
		using limits_is_signed =
		  std::bool_constant<daw::numeric_limits<T>::is_signed>;

		template<typename T>
		using limits_is_exact =
		  std::bool_constant<daw::numeric_limits<T>::is_exact>;

	} // namespace arith_traits_details

	template<typename T>
	using is_floating_point =
	  std::conjunction<std::negation<is_integral<T>>,
	                   arith_traits_details::limits_is_signed<T>,
	                   std::negation<arith_traits_details::limits_is_exact<T>>>;

	template<typename T>
	inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

	static_assert( is_floating_point_v<float> );
	static_assert( not is_floating_point_v<int> );

	template<typename T>
	using is_number = std::disjunction<is_integral<T>, is_floating_point<T>>;

	template<typename T>
	inline constexpr bool is_number_v = is_number<T>::value;

	static_assert( is_number_v<float> );
	static_assert( is_number_v<int> );
	static_assert( not is_number_v<is_integral<int>> );

	template<typename T>
	using is_signed =
	  std::conjunction<is_number<T>, arith_traits_details::limits_is_signed<T>>;

	template<typename T>
	inline constexpr bool is_signed_v = is_signed<T>::value;

	static_assert( is_signed_v<int> );
	static_assert( is_signed_v<float> );
	static_assert( not is_signed_v<unsigned> );

	template<typename T>
	using is_unsigned =
	  std::conjunction<is_integral<T>,
	                   std::negation<arith_traits_details::limits_is_signed<T>>>;

	template<typename T>
	inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

	static_assert( not is_unsigned_v<int> );
	static_assert( not is_unsigned_v<float> );
	static_assert( is_unsigned_v<unsigned> );

	template<typename T>
	using is_arithmetic =
	  std::disjunction<is_number<T>, std::is_enum<T>,
	                   conditional_t<daw::numeric_limits<T>::is_specialized,
	                                 std::true_type, std::false_type>>;

	template<typename T>
	inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

	template<typename T>
	struct make_unsigned : std::make_unsigned<T> {};

	template<typename T>
	struct make_signed : std::make_signed<T> {};

#if defined( DAW_HAS_INT128 )
	template<>
	struct make_unsigned<__uint128_t> {
		using type = __uint128_t;
	};

	template<>
	struct make_unsigned<__uint128_t const> {
		using type = __uint128_t const;
	};

	template<>
	struct make_unsigned<__uint128_t volatile> {
		using type = __uint128_t volatile;
	};

	template<>
	struct make_unsigned<__uint128_t const volatile> {
		using type = __uint128_t const volatile;
	};

	template<>
	struct make_unsigned<__int128_t> {
		using type = __uint128_t;
	};

	template<>
	struct make_unsigned<__int128_t const> {
		using type = __uint128_t const;
	};

	template<>
	struct make_unsigned<__int128_t volatile> {
		using type = __uint128_t volatile;
	};

	template<>
	struct make_unsigned<__int128_t const volatile> {
		using type = __uint128_t const volatile;
	};

	template<>
	struct make_signed<__uint128_t> {
		using type = __int128_t;
	};

	template<>
	struct make_signed<__uint128_t const> {
		using type = __int128_t const;
	};

	template<>
	struct make_signed<__uint128_t volatile> {
		using type = __int128_t volatile;
	};

	template<>
	struct make_signed<__uint128_t const volatile> {
		using type = __int128_t const volatile;
	};

	template<>
	struct make_signed<__int128_t> {
		using type = __int128_t;
	};

	template<>
	struct make_signed<__int128_t const> {
		using type = __int128_t const;
	};

	template<>
	struct make_signed<__int128_t volatile> {
		using type = __int128_t volatile;
	};

	template<>
	struct make_signed<__int128_t const volatile> {
		using type = __int128_t const volatile;
	};
#endif

	template<typename T>
	using make_unsigned_t = typename make_unsigned<T>::type;

	template<typename T>
	using make_signed_t = typename make_signed<T>::type;

	template<typename T>
	struct is_system_integral : std::is_integral<T> {};

#if defined( DAW_HAS_INT128 )
	template<>
	struct is_system_integral<__uint128_t> : std::true_type {};

	template<>
	struct is_system_integral<__int128_t> : std::true_type {};
#endif
	template<typename T>
	inline constexpr bool is_system_integral_v = is_system_integral<T>::value;

	template<typename T, std::size_t BitSize>
	using is_same_size = std::bool_constant<( ( sizeof( T ) * 8 ) == BitSize )>;

	template<typename T, std::size_t BitSize>
	inline constexpr bool is_same_size_v = is_same_size<T, BitSize>::value;

	template<std::size_t /*BitSize*/>
	struct unsupported_int_size;

	template<std::size_t BitSize>
	using intN_t = conditional_t<
	  is_same_size_v<char, BitSize>, char,
	  conditional_t<
	    is_same_size_v<short, BitSize>, short,
	    conditional_t<
	      is_same_size_v<int, BitSize>, int,
	      conditional_t<
	        is_same_size_v<long, BitSize>, long,
	        conditional_t<is_same_size_v<long long, BitSize>, long long,
	                      unsupported_int_size<BitSize>>>>>>;
	template<std::size_t BitSize>
	using uintN_t = conditional_t<
	  is_same_size_v<char, BitSize>, unsigned char,
	  conditional_t<
	    is_same_size_v<short, BitSize>, unsigned short,
	    conditional_t<
	      is_same_size_v<int, BitSize>, unsigned int,
	      conditional_t<
	        is_same_size_v<long, BitSize>, unsigned long,
	        conditional_t<is_same_size_v<unsigned long long, BitSize>, long long,
	                      unsupported_int_size<BitSize>>>>>>;

	template<typename T>
	inline constexpr auto max_value = numeric_limits<T>::max( );

	template<typename T>
	inline constexpr auto min_value = numeric_limits<T>::min( );

	template<typename T>
	inline constexpr auto lowest_value = numeric_limits<T>::lowest( );
} // namespace daw

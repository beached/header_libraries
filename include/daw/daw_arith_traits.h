// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/impl/daw_gcc_clang_int128.h"
#include "daw/impl/daw_int128_check.h"
#include "daw/impl/daw_msvc_int128.h"
#include "daw/impl/daw_numeric_limits.h"
#include "daw/traits/daw_traits_conditional.h"

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw {
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
	struct make_unsigned<uint128_t> {
		using type = uint128_t;
	};

	template<>
	struct make_unsigned<uint128_t const> {
		using type = uint128_t const;
	};

	template<>
	struct make_unsigned<uint128_t volatile> {
		using type = uint128_t volatile;
	};

	template<>
	struct make_unsigned<uint128_t const volatile> {
		using type = uint128_t const volatile;
	};

	template<>
	struct make_unsigned<int128_t> {
		using type = uint128_t;
	};

	template<>
	struct make_unsigned<int128_t const> {
		using type = uint128_t const;
	};

	template<>
	struct make_unsigned<int128_t volatile> {
		using type = uint128_t volatile;
	};

	template<>
	struct make_unsigned<int128_t const volatile> {
		using type = uint128_t const volatile;
	};

	template<>
	struct make_signed<uint128_t> {
		using type = int128_t;
	};

	template<>
	struct make_signed<uint128_t const> {
		using type = int128_t const;
	};

	template<>
	struct make_signed<uint128_t volatile> {
		using type = int128_t volatile;
	};

	template<>
	struct make_signed<uint128_t const volatile> {
		using type = int128_t const volatile;
	};

	template<>
	struct make_signed<int128_t> {
		using type = int128_t;
	};

	template<>
	struct make_signed<int128_t const> {
		using type = int128_t const;
	};

	template<>
	struct make_signed<int128_t volatile> {
		using type = int128_t volatile;
	};

	template<>
	struct make_signed<int128_t const volatile> {
		using type = int128_t const volatile;
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
	struct is_system_integral<uint128_t> : std::true_type {};

	template<>
	struct is_system_integral<int128_t> : std::true_type {};
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

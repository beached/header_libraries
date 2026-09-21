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
#include "daw/daw_attributes.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/impl/daw_gcc_clang_int128.h"
#include "daw/impl/daw_int128_check.h"
#include "daw/impl/daw_msvc_int128.h"
#include "daw/impl/daw_numeric_limits.h"
#include "daw/traits/daw_traits_conditional.h"
#include "daw/traits/daw_traits_first_type.h"
#include "daw/traits/daw_traits_nth_element.h"

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace daw {
	template<typename T>
	inline constexpr bool is_integral_v = daw::numeric_limits<T>::is_integer;

	template<typename T>
	using is_integral = std::bool_constant<is_integral_v<T>>;

	static_assert( is_integral_v<int> );
	static_assert( not is_integral_v<float> );

	template<typename T>
	inline constexpr bool is_floating_point_v =
	  not is_integral_v<T> and daw::numeric_limits<T>::is_signed and
	  not daw::numeric_limits<T>::is_exact;

	template<typename T>
	using is_floating_point = std::bool_constant<is_floating_point_v<T>>;

	static_assert( is_floating_point_v<float> );
	static_assert( not is_floating_point_v<int> );

	template<typename T>
	inline constexpr bool is_number_v =
	  is_integral_v<T> or is_floating_point_v<T>;

	template<typename T>
	using is_number = std::bool_constant<is_number_v<T>>;

	static_assert( is_number_v<float> );
	static_assert( is_number_v<int> );
	static_assert( not is_number_v<is_integral<int>> );

	template<typename T>
	inline constexpr bool is_signed_v =
	  is_number_v<T> and daw::numeric_limits<T>::is_signed;

	template<typename T>
	using is_signed = std::bool_constant<is_signed_v<T>>;

	static_assert( is_signed_v<int> );
	static_assert( is_signed_v<float> );
	static_assert( not is_signed_v<unsigned> );

	template<typename T>
	inline constexpr bool is_unsigned_v =
	  is_integral_v<T> and not daw::numeric_limits<T>::is_signed;

	template<typename T>
	using is_unsigned = std::bool_constant<is_unsigned_v<T>>;

	static_assert( not is_unsigned_v<int> );
	static_assert( not is_unsigned_v<float> );
	static_assert( is_unsigned_v<unsigned> );

	template<typename T>
	inline constexpr bool is_arithmetic_v =
	  is_number_v<T> or std::is_enum_v<T> or
	  daw::numeric_limits<T>::is_specialized;

	template<typename T>
	using is_arithmetic = std::bool_constant<is_arithmetic_v<T>>;

	template<typename T, typename = void>
	inline constexpr bool has_std_make_unsigned_v =
	  ( std::is_integral_v<T> or std::is_enum_v<T> ) and
	  not std::is_same_v<std::remove_cv_t<T>, bool>;

	template<typename T, typename = void>
	inline constexpr bool has_std_make_signed_v =
	  ( std::is_integral_v<T> or std::is_enum_v<T> ) and
	  not std::is_same_v<std::remove_cv_t<T>, bool>;

	template<typename T, typename = void>
	struct make_unsigned;

	template<typename T>
	struct make_unsigned<T, std::enable_if_t<has_std_make_unsigned_v<T>>>
	  : std::make_unsigned<T> {};

	template<typename, typename = void>
	struct make_signed;

	template<typename T>
	struct make_signed<T, std::enable_if_t<has_std_make_signed_v<T>>>
	  : std::make_signed<T> {};

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
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr make_unsigned_t<T>
	as_unsigned( T const &value ) {
		return static_cast<make_unsigned_t<T>>( value );
	}

	template<typename T>
	using make_signed_t = typename make_signed<T>::type;

	template<typename T>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr make_signed_t<T>
	as_signed( T const &value ) {
		return static_cast<make_signed_t<T>>( value );
	}

	template<typename T, typename = void>
	inline constexpr bool has_make_unsigned_v = false;

	template<typename T>
	inline constexpr bool
	  has_make_unsigned_v<T, std::void_t<typename make_unsigned<T>::type>> = true;

	template<typename T, typename = void>
	inline constexpr bool has_make_signed_v = false;

	template<typename T>
	inline constexpr bool
	  has_make_signed_v<T, std::void_t<typename make_signed<T>::type>> = true;

	namespace arith_traits_impl {
		template<typename T, bool = has_make_unsigned_v<T>>
		struct try_make_unsigned {
			using type = T;
		};

		template<typename T>
		struct try_make_unsigned<T, true> : make_unsigned<T> {};

		template<typename T, bool = has_make_signed_v<T>>
		struct try_make_signed {
			using type = T;
		};

		template<typename T>
		struct try_make_signed<T, true> : make_signed<T> {};
	} // namespace arith_traits_impl

	template<typename T>
	using try_make_unsigned_t =
	  typename arith_traits_impl::try_make_unsigned<T>::type;
	template<typename T>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	try_as_unsigned( T const &value ) {
		return static_cast<try_make_unsigned_t<T>>( value );
	}

	template<typename T>
	using try_make_signed_t =
	  typename arith_traits_impl::try_make_signed<T>::type;
	template<typename T>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	try_as_signed( T const &value ) {
		return static_cast<try_make_signed_t<T>>( value );
	}

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
	using is_same_size = std::bool_constant<( bit_count_v<T> == BitSize )>;

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

	template<typename T>
	inline constexpr auto max_digits10 = numeric_limits<T>::max_digits10;

	template<typename T>
	inline constexpr auto digits10 = numeric_limits<T>::digits10;

	template<typename T>
	inline constexpr auto digits = numeric_limits<T>::digits;

	namespace next_wider_impl {
		template<typename T>
		struct type_identity {
			using type = T;
		};

		template<typename T, std::size_t sz = sizeof( T ),
		         bool is_integral = daw::is_integral_v<T>,
		         bool is_signed = daw::is_signed_v<T>>
		DAW_CONSTEVAL auto next_wider_helper( ) {
			if constexpr( not is_integral ) {
				return type_identity<T>{ };
			} else if constexpr( sz == 1 ) {
				if constexpr( is_signed ) {
					return type_identity<std::int16_t>{ };
				} else {
					return type_identity<std::uint16_t>{ };
				}
			} else if constexpr( sz == 2 ) {
				if constexpr( is_signed ) {
					return type_identity<std::int32_t>{ };
				} else {
					return type_identity<std::uint32_t>{ };
				}
			} else if constexpr( sz == 4 ) {
				if constexpr( is_signed ) {
					return type_identity<std::int64_t>{ };
				} else {
					return type_identity<std::uint64_t>{ };
				}
			} else if constexpr( sz == 8 ) {
#if defined( DAW_HAS_INT128 )
				if constexpr( is_signed ) {
					return type_identity<daw::int128_t>{ };
				} else {
					return type_identity<daw::uint128_t>{ };
				}
#else
				return type_identity<T>{ };
#endif
			} else {
				return type_identity<T>{ };
			}
		}
	} // namespace next_wider_impl

	template<typename T, std::size_t sz = sizeof( T ),
	         bool is_integral = daw::is_integral_v<T>,
	         bool is_signed = daw::is_signed_v<T>>
	using next_wider_t =
	  typename decltype( next_wider_impl::next_wider_helper<T, sz, is_integral,
	                                                        is_signed>( ) )::type;

	/// For std integers do not make the type larger than 64bit
	template<typename T>
	using next_wider_fast_t =
	  std::conditional_t<std::is_integral_v<T>,
	                     next_wider_t<T, ( sizeof( T ) < 4 ? sizeof( T ) : 4 )>,
	                     T>;

	template<typename T>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	as_next_wider( T const &value ) {
		return static_cast<next_wider_t<T>>( value );
	}

	namespace next_wider_impl {
		template<typename T, typename... Ts>
		inline constexpr std::size_t max_size_v = [] {
			std::size_t result = sizeof( T );
			( ( result = result < sizeof( Ts ) ? sizeof( Ts ) : result ), ... );
			return result;
		}( );

		template<typename T, typename... Ts>
		inline constexpr bool any_signed_v =
		  daw::is_signed_v<T> or ( daw::is_signed_v<Ts> or ... );

		template<typename T, typename... Ts>
		inline constexpr bool all_integral_v =
		  daw::is_integral_v<T> and ( daw::is_integral_v<Ts> and ... );

		template<typename T, typename... Ts>
		inline constexpr bool all_std_integral_v =
		  std::is_integral_v<T> and ( std::is_integral_v<Ts> and ... );

		template<typename... Ts>
		inline constexpr std::size_t widest_type_idx = [] {
			static_assert( sizeof...( Ts ) > 0 );
			std::size_t max_idx = 0;
			std::size_t max_sz = 0;
			std::size_t cur_idx = 0;
			auto const update_max_idx = [&]( std::size_t sz ) {
				if( sz > max_sz ) {
					max_sz = sz;
					max_idx = cur_idx;
				}
				++cur_idx;
				return true;
			};
			(void)( update_max_idx( sizeof( Ts ) ) and ... );
			return max_idx;
		}( );
	} // namespace next_wider_impl

	template<typename... Ts>
	using widest_type_t =
	  traits::nth_element<next_wider_impl::widest_type_idx<Ts...>, Ts...>;

	template<typename... Ts>
	using next_wider_fast_widest_t =
	  typename decltype( next_wider_impl::next_wider_helper<
	                     daw::traits::first_type<Ts...>,
	                     next_wider_impl::all_std_integral_v<
	                       daw::traits::first_type<Ts...>, Ts...>
	                       ? ( next_wider_impl::max_size_v<
	                               daw::traits::first_type<Ts...>, Ts...> < 4
	                             ? next_wider_impl::max_size_v<
	                                 daw::traits::first_type<Ts...>, Ts...>
	                             : 4 )
	                       : sizeof( daw::traits::first_type<Ts...> ),
	                     next_wider_impl::all_std_integral_v<
	                       daw::traits::first_type<Ts...>, Ts...>,
	                     next_wider_impl::any_signed_v<
	                       daw::traits::first_type<Ts...>, Ts...>>( ) )::type;
} // namespace daw

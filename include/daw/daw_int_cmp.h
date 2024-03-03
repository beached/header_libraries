// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// This is an implementation of [p586] Integer Comparison Functions
// https://wg21.link/p0586

#pragma once

#include "daw_attributes.h"

#include <cstddef>
#include <limits>
#include <type_traits>

namespace daw {
	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_equal( T t, U u ) noexcept {
		using UT = std::make_unsigned_t<T>;
		using UU = std::make_unsigned_t<U>;
		if constexpr( std::is_signed_v<T> == std::is_signed_v<U> ) {
			return t == u;
		} else if constexpr( std::is_signed_v<T> ) {
			return t < 0 ? false : static_cast<UT>( t ) == u;
		} else {
			return u < 0 ? false : t == static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_not_equal( T t, U u ) noexcept {
		using UT = std::make_unsigned_t<T>;
		using UU = std::make_unsigned_t<U>;
		if constexpr( std::is_signed_v<T> == std::is_signed_v<U> ) {
			return t != u;
		} else if constexpr( std::is_signed_v<T> ) {
			return t < 0 ? true : static_cast<UT>( t ) != u;
		} else {
			return u < 0 ? true : t != static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_less( T t, U u ) noexcept {
		using UT = std::make_unsigned_t<T>;
		using UU = std::make_unsigned_t<U>;
		if constexpr( std::is_signed_v<T> == std::is_signed_v<U> ) {
			return t < u;
		} else if constexpr( std::is_signed_v<T> ) {
			return t < 0 ? true : static_cast<UT>( t ) < u;
		} else {
			return u < 0 ? false : t < static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_greater( T t, U u ) noexcept {
		return cmp_less( u, t );
	}

	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_less_equal( T t, U u ) noexcept {
		return not cmp_less( u, t );
	}

	template<typename T, typename U,
	         std::enable_if_t<std::is_integral_v<T> and std::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_greater_equal( T t, U u ) noexcept {
		return not cmp_less( t, u );
	}

	template<typename R, typename T,
	         std::enable_if_t<std::is_integral_v<R> and std::is_integral_v<T>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool in_range( T t ) noexcept {
		return cmp_greater_equal( t, std::numeric_limits<R>::min( ) ) and
		       cmp_less_equal( t, std::numeric_limits<R>::max( ) );
	}

	template<typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool signbit( T t ) {
		if constexpr( std::is_unsigned_v<T> ) {
			return false;
		}
		return t < 0;
	}
} // namespace daw

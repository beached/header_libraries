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

#include "daw/daw_arith_traits.h"
#include "daw/daw_attributes.h"

#include <cstddef>
#include <limits>
#include <type_traits>

namespace daw {
	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_equal( T t, U u ) noexcept {
		using UT = daw::make_unsigned_t<T>;
		using UU = daw::make_unsigned_t<U>;
		if constexpr( daw::is_signed_v<T> == daw::is_signed_v<U> ) {
			return t == u;
		} else if constexpr( daw::is_signed_v<T> ) {
			return t < 0 ? false : static_cast<UT>( t ) == u;
		} else {
			return u < 0 ? false : t == static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_not_equal( T t, U u ) noexcept {
		using UT = daw::make_unsigned_t<T>;
		using UU = daw::make_unsigned_t<U>;
		if constexpr( daw::is_signed_v<T> == daw::is_signed_v<U> ) {
			return t != u;
		} else if constexpr( daw::is_signed_v<T> ) {
			return t < 0 ? true : static_cast<UT>( t ) != u;
		} else {
			return u < 0 ? true : t != static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_less( T t, U u ) noexcept {
		using UT = daw::make_unsigned_t<T>;
		using UU = daw::make_unsigned_t<U>;
		if constexpr( daw::is_signed_v<T> == daw::is_signed_v<U> ) {
			return t < u;
		} else if constexpr( daw::is_signed_v<T> ) {
			return t < 0 ? true : static_cast<UT>( t ) < u;
		} else {
			return u < 0 ? false : t < static_cast<UU>( u );
		}
	}

	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_greater( T t, U u ) noexcept {
		return cmp_less( u, t );
	}

	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_less_equal( T t, U u ) noexcept {
		return not cmp_less( u, t );
	}

	template<typename T, typename U,
	         std::enable_if_t<daw::is_integral_v<T> and daw::is_integral_v<U>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool cmp_greater_equal( T t, U u ) noexcept {
		return not cmp_less( t, u );
	}

	template<typename R, typename T,
	         std::enable_if_t<daw::is_integral_v<R> and daw::is_integral_v<T>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool in_range( T t ) noexcept {
		if constexpr( std::is_same_v<R, T> ) {
			return true;
		} else {
			return cmp_greater_equal( t, lowest_value<R> ) and
			       cmp_less_equal( t, max_value<R> );
		}
	}

	template<typename T,
	         std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool signbit( T t ) {
		if constexpr( daw::is_unsigned_v<T> ) {
			return false;
		} else {
			return t < 0;
		}
	}
} // namespace daw

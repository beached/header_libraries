// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#include <memory>
#include <type_traits>

#if defined( __cpp_constexpr_dynamic_alloc )
#if __cpp_constexpr_dynamic_alloc >= 201907L
#define DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC
#endif
#endif

#if defined( DAW_CPP20_CONSTEXPR_DYNAMIC_ALLOC )
#define DAW_CX_DTOR constexpr
#define DAW_CPP20_CX_DTOR constexpr
#define DAW_CPP20_CX_ALLOC constexpr
#else
#define DAW_CX_DTOR
#define DAW_CPP20_CX_DTOR
#define DAW_CPP20_CX_ALLOC
#endif

#if defined( __cpp_lib_three_way_comparison )
#if __cpp_lib_three_way_comparison >= 201907L
#if __has_include( <compare> )
#include <compare>
#define DAW_HAS_CPP20_3WAY_COMPARE
#endif
#endif
#endif

#if defined( __cpp_concepts )
#if __cpp_concepts >= 201907L
#if not defined( DAW_CPP20_CONCEPTS )
#define DAW_CPP20_CONCEPTS
#endif
#endif
#endif

namespace daw {
	template<typename T>
	inline constexpr bool is_unbounded_array_v = false;

	template<typename T>
	inline constexpr bool is_unbounded_array_v<T[]> = true;

	template<typename T>
	using is_unbounded_array = std::bool_constant<is_unbounded_array_v<T>>;

	namespace cpp_20_details {
#if defined( DAW_CPP20_CONCEPTS )
		template<typename T>
		inline constexpr bool has_pointer_traits_to_address_v =
		  requires( T const &p ) {
			std::pointer_traits<T>::to_address( p );
		};
#else
		template<typename T, typename = void>
		inline constexpr bool has_pointer_traits_to_address_v = false;

		template<typename T>
		inline constexpr bool has_pointer_traits_to_address_v<
		  T, std::void_t<decltype( std::pointer_traits<T>::to_address(
		       std::declval<T const &>( ) ) )>> = true;
#endif
	} // namespace cpp_20_details

	template<typename T>
	constexpr T *to_address( T *p ) noexcept {
		static_assert( not std::is_function_v<T> );
		return p;
	}

	template<typename T>
	constexpr auto to_address( T const &p ) noexcept {
		if constexpr( cpp_20_details::has_pointer_traits_to_address_v<T> ) {
			return std::pointer_traits<T>::to_address( p );
		} else {
			return daw::to_address( p.operator->( ) );
		}
	}
} // namespace daw
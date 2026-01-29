// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <type_traits>
#include <utility>

namespace daw {
	template<class T, class U>
	constexpr auto &&forward_like( U &&x ) noexcept {
		constexpr bool is_adding_const =
		  std::is_const_v<std::remove_reference_t<T>>;
		if constexpr( std::is_lvalue_reference_v<T &&> ) {
			if constexpr( is_adding_const ) {
				return std::as_const( x );
			} else {
				return static_cast<U &>( x );
			}
		} else {
			if constexpr( is_adding_const ) {
				return std::move( std::as_const( x ) );
			} else {
				return std::move( x );
			}
		}
	}

	template<typename T, typename U>
	using forward_like_t = std::conditional_t<
	  std::is_lvalue_reference_v<T &&>,
	  std::conditional_t<
	    std::is_const_v<std::remove_reference_t<T>>,
	    std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<U>>>,
	    std::add_lvalue_reference_t<std::remove_reference_t<U>>>,
	  std::conditional_t<
	    std::is_const_v<std::remove_reference_t<T>>,
	    std::add_rvalue_reference_t<std::add_const_t<std::remove_reference_t<U>>>,
	    std::add_rvalue_reference_t<std::remove_reference_t<U>>>>;
} // namespace daw

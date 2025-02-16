// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#if defined( DAW_HAS_CPP20_3WAY )
#include "daw/daw_undefined.h"

#include <bit>
#include <compare>

namespace daw {
	namespace order_impl {
		constexpr auto order_to_integer( std::strong_ordering order ) {
			return std::bit_cast<signed char>( order );
		}

		constexpr auto order_to_integer( std::partial_ordering order ) {
			return std::bit_cast<signed char>( order );
		}

		constexpr auto order_to_integer( std::weak_ordering order ) {
			return std::bit_cast<signed char>( order );
		}
	} // namespace order_impl

	enum class strong_ordering {
		equal = order_impl::order_to_integer( std::strong_ordering::equal ),
		equivalent =
		  order_impl::order_to_integer( std::strong_ordering::equivalent ),
		less = order_impl::order_to_integer( std::strong_ordering::less ),
		greater = order_impl::order_to_integer( std::strong_ordering::greater )
	};

	enum class partial_ordering {
		equivalent =
		  order_impl::order_to_integer( std::partial_ordering::equivalent ),
		less = order_impl::order_to_integer( std::partial_ordering::less ),
		greater = order_impl::order_to_integer( std::partial_ordering::greater ),
		unordered = order_impl::order_to_integer( std::partial_ordering::unordered )
	};

	enum class weak_ordering {
		equivalent = order_impl::order_to_integer( std::weak_ordering::equivalent ),
		less = order_impl::order_to_integer( std::weak_ordering::less ),
		greater = order_impl::order_to_integer( std::weak_ordering::greater )
	};

	namespace order_impl {
		constexpr auto order_to_integer( strong_ordering order ) {
			return static_cast<signed char>( order );
		}

		constexpr auto order_to_integer( partial_ordering order ) {
			return static_cast<signed char>( order );
		}

		constexpr auto order_to_integer( weak_ordering order ) {
			return static_cast<signed char>( order );
		}
	} // namespace order_impl

	constexpr strong_ordering order_to_enum( std::strong_ordering order ) {
		return strong_ordering{ order_impl::order_to_integer( order ) };
	}

	constexpr partial_ordering order_to_enum( std::partial_ordering order ) {
		return partial_ordering{ order_impl::order_to_integer( order ) };
	}

	constexpr weak_ordering order_to_enum( std::weak_ordering order ) {
		return weak_ordering{ order_impl::order_to_integer( order ) };
	}

	template<typename Order>
	constexpr bool equal_or_equivalent( Order order ) {
		return order_impl::order_to_integer( order ) == 0;
	}

	template<typename Order>
	inline constexpr auto equal_or_equivalent_v = true;

	template<>
	inline constexpr auto equal_or_equivalent_v<std::strong_ordering> =
	  std::strong_ordering::equal;

	template<>
	inline constexpr auto equal_or_equivalent_v<std::partial_ordering> =
	  std::partial_ordering::equivalent;

	template<>
	inline constexpr auto equal_or_equivalent_v<std::weak_ordering> =
	  std::weak_ordering::equivalent;
} // namespace daw
#endif

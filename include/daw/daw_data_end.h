// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/declval.h>
#include <daw/stdinc/range_access.h>
#include <daw/stdinc/void_t.h>

namespace daw {
	namespace utility_details {
		template<typename T, typename = void>
		inline constexpr bool has_data_end_v = false;

		template<typename T>
		inline constexpr bool has_data_end_v<
		  T, std::void_t<decltype( std::declval<T &>( ).data_end( ) )>> = true;

		template<typename T, typename = void>
		inline constexpr bool has_data_v = false;

		template<typename T>
		inline constexpr bool
		  has_data_v<T, std::void_t<decltype( std::declval<T &>( ).data( ) )>> =
		    true;

		struct AlwaysNull {};
	} // namespace utility_details
} // namespace daw

namespace daw {
	template<typename Container>
	[[nodiscard]] inline constexpr auto data( Container &&c ) {
		if constexpr( utility_details::has_data_v<Container> ) {
			return c.data( );
		} else {
			return std::data( c );
		}
	}

	template<typename Container>
	[[nodiscard]] inline constexpr auto data_end( Container &&c ) {
		if constexpr( utility_details::has_data_end_v<Container> ) {
			return c.data_end( );
		} else if constexpr( utility_details::has_data_v<Container> ) {
			return std::next( c.data( ),
			                  static_cast<std::ptrdiff_t>( std::size( c ) ) );
		} else {
			return std::next( std::data( c ),
			                  static_cast<std::ptrdiff_t>( std::size( c ) ) );
		}
	}
} // namespace daw

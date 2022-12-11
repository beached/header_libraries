// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"
#include "daw_traits.h"

#include <type_traits>

namespace daw {
	template<typename T>
	struct use_aggregate_construction {
		using type = T;

		use_aggregate_construction( ) = delete;
	};

	namespace construct_details {
		template<typename T>
		constexpr int should_use_aggregate_construction_test(
		  use_aggregate_construction<T> const & ) noexcept;

		template<typename T>
		using should_use_aggregate_construction_detect =
		  decltype( should_use_aggregate_construction_test( std::declval<T>( ) ) );

		template<typename T>
		constexpr bool should_use_aggregate_construction_v =
		  daw::is_detected_v<should_use_aggregate_construction_detect, T>;
	} // namespace construct_details

	///
	/// @brief Construct a value.  If normal ( ) construction does not work
	///	try aggregate.
	/// @tparam T type of value to construct
	template<typename T>
	struct construct_a_t {
		template<typename... Args>
		[[nodiscard]] inline constexpr auto operator( )( Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<T, Args...> )
		    -> std::enable_if_t<std::is_constructible_v<T, Args...>, T> {
			if constexpr( std::is_aggregate_v<T> ) {
				return T{ DAW_FWD( args )... };
			} else {
				return T( DAW_FWD( args )... );
			}
		}

		template<typename... Args>
		[[nodiscard]] inline constexpr auto operator( )( Args &&...args ) const
		  noexcept( traits::is_nothrow_list_constructible_v<T, Args...> )
		    -> std::enable_if_t<
		      std::conjunction_v<
		        traits::static_not<std::is_constructible<T, Args...>>,
		        traits::is_list_constructible<T, Args...>>,
		      T> {

			return T{ DAW_FWD( args )... };
		}
	};

	template<typename T>
	struct construct_a_t<daw::use_aggregate_construction<T>> {

		template<typename... Args>
		[[nodiscard]] inline constexpr auto operator( )( Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<T, Args...> ) -> T {

			return T{ DAW_FWD( args )... };
		}
	};

	template<typename T>
	inline constexpr construct_a_t<T> construct_a = construct_a_t<T>{ };

	namespace construct_details {
		template<typename T>
		inline constexpr construct_a_t<T> construct_a = construct_a_t<T>{ };

		template<typename T, typename... Args>
		using can_construct_a_detect =
		  decltype( std::declval<daw::construct_a_t<T>>( )(
		    std::declval<Args>( )... ) );
	} // namespace construct_details

	template<typename T, typename... Args>
	inline constexpr bool can_construct_a_v =
	  is_detected_v<construct_details::can_construct_a_detect, T, Args...>;

} // namespace daw

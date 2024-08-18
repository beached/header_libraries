// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_apply.h"
#include "daw/daw_compiler_fixups.h"
#include "daw/daw_move.h"

#include <type_traits>

namespace daw::algorithm {
	namespace accum_impl {
		template<typename, typename = void>
		inline constexpr bool is_container_like_v = false;

		template<typename C>
		inline constexpr bool is_container_like_v<
		  C,
		  std::void_t<decltype( ( (void)std::declval<C const &>( ).begin( ) ),
		                        ( (void)std::declval<C const &>( ).end( ) ) )>> =
		  true;

		struct plus {
			template<typename L, typename R>
			constexpr decltype( auto ) operator( )( L &&l, R &&r ) const {
				return DAW_FWD( l ) + DAW_FWD( r );
			}
		};
	} // namespace accum_impl
	template<typename InputIterator, typename T>
	constexpr auto
	accumulate( InputIterator first, InputIterator last, T init ) noexcept {
		DAW_UNSAFE_BUFFER_FUNC_START
		for( ; first != last; ++first ) {
			init = std::move( init ) + *first;
		}
		DAW_UNSAFE_BUFFER_FUNC_STOP
		return init;
	}

	/***
	 * Accumulate values in range
	 * @tparam InputIterator type of first iterator argument
	 * @tparam LastType type of sentinal marking end of range
	 * @tparam T initial value to start accumulating at
	 * @tparam BinaryOperation Callable that takes the current sum and next value
	 * and returns the new sum
	 * @param first beginning position in range
	 * @param last end of range
	 * @param init initial value of sum
	 * @param binary_op operation to run
	 * @return sum of values
	 */
	template<typename InputIterator,
	         typename LastType,
	         typename T,
	         typename BinaryOperation = accum_impl::plus,
	         std::enable_if_t<not accum_impl::is_container_like_v<InputIterator>,
	                          std::nullptr_t> = nullptr>
	constexpr auto accumulate(
	  InputIterator first,
	  LastType last,
	  T init,
	  BinaryOperation binary_op =
	    BinaryOperation{ } ) noexcept( noexcept( binary_op( std::move( init ),
	                                                        *first ) ) ) {

		while( first != last ) {
			init = daw::invoke( binary_op, std::move( init ), *first );
			++first;
		}
		return init;
	}
} // namespace daw::algorithm

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_exchange.h"
#include "daw_move.h"

#include <daw/stdinc/declval.h>
#include <daw/stdinc/remove_cvref.h>
#include <daw/stdinc/void_t.h>
#include <type_traits>

namespace daw {
	namespace swap_details {
		template<typename T, typename U = T, typename = void>
		inline constexpr bool has_member_swap_v = false;

		template<typename T, typename U>
		inline constexpr bool
		  has_member_swap_v<T, U,
		                    std::void_t<decltype( std::declval<T &>( ).swap(
		                      std::declval<U &>( ) ) )>> = true;

		template<typename T, typename U = T, typename = void>
		inline constexpr bool has_adl_swap_v = false;

		template<typename T, typename U>
		inline constexpr bool
		  has_adl_swap_v<T, U,
		                 std::void_t<decltype( swap(
		                   std::declval<T &>( ), std::declval<U &>( ) ) )>> = true;
	} // namespace swap_details

	template<typename T>
	constexpr void cswap( T &&lhs, T &&rhs ) noexcept(
	  std::conjunction_v<std::is_nothrow_move_assignable<T>,
	                     std::is_nothrow_move_constructible<T>> ) {
		static_assert( not std::is_const_v<std::remove_reference_t<T>>,
		               "Cannot swap const values" );
		if constexpr( std::is_trivial_v<std::remove_reference_t<T>> ) {
			auto tmp = lhs;
			lhs = daw::exchange( rhs, tmp );
		} else if constexpr( std::is_scalar_v<std::remove_reference_t<T>> ) {
			auto tmp = DAW_MOVE( lhs );
			lhs = daw::exchange( DAW_MOVE( rhs ), DAW_MOVE( tmp ) );
		} else if constexpr( swap_details::has_member_swap_v<
		                       std::remove_reference_t<T>> ) {
			lhs.swap( rhs );
		} else if constexpr( swap_details::has_adl_swap_v<
		                       std::remove_reference_t<T>> ) {
			swap( lhs, rhs );
		} else {
			using std::swap;
			swap( lhs, rhs );
		}
	}

	template<typename ForwardIterator1, typename ForwardIterator2>
	constexpr void
	iter_swap( ForwardIterator1 lhs, ForwardIterator2 rhs ) noexcept(
	  std::conjunction_v<std::is_nothrow_move_assignable<decltype( *lhs )>,
	                     std::is_nothrow_move_assignable<decltype( *rhs )>> ) {
		auto tmp = DAW_MOVE( *lhs );
		*lhs = DAW_MOVE( *rhs );
		*rhs = DAW_MOVE( tmp );
	}

	template<typename ForwardIterator1, typename ForwardIterator2>
	constexpr ForwardIterator2 swap_ranges(
	  ForwardIterator1 first1, ForwardIterator1 last1,
	  ForwardIterator2 first2 ) noexcept( noexcept( daw::iter_swap( first1,
	                                                                first2 ) ) ) {
		while( first1 != last1 ) {
			daw::iter_swap( *first1, *first2 );
			++first1;
			++first2;
		}
		return first2;
	}

	template<typename T, size_t N>
	constexpr void cswap( std::array<T, N> &lhs, std::array<T, N> &rhs ) noexcept(
	  std::conjunction_v<std::is_nothrow_move_assignable<T>,
	                     std::is_nothrow_move_constructible<T>> ) {

		daw::swap_ranges( lhs.begin( ), lhs.end( ), rhs.begin( ) );
	}
} // namespace daw

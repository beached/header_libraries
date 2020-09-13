// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "cpp_17.h"
#include "daw_exchange.h"
#include "daw_move.h"
#include "daw_swap.h"

namespace daw {
	namespace swap_details {
		template<typename T, typename U>
		using has_swap_test =
		  decltype( std::declval<T &>( ).swap( std::declval<U &>( ) ) );

		template<typename T, typename U = T>
		inline constexpr bool has_member_swap_v =
		  daw::is_detected_v<has_swap_test, T, U>;
	} // namespace swap_details

	template<typename T, typename U>
	constexpr void cswap( std::pair<T, U> &lhs, std::pair<T, U> &rhs ) noexcept(
	  std::is_nothrow_move_assignable_v<std::pair<T, U>> ) {
		auto tmp = daw::move( lhs );
		lhs = daw::exchange( rhs, daw::move( tmp ) );
	}

	template<typename... Args>
	constexpr void
	cswap( std::tuple<Args...> &lhs, std::tuple<Args...> &rhs ) noexcept(
	  std::is_nothrow_move_assignable_v<std::tuple<Args...>> ) {
		auto tmp = daw::move( lhs );
		lhs = daw::exchange( rhs, daw::move( tmp ) );
	}

	template<typename... Args>
	constexpr void
	cswap( std::variant<Args...> &lhs, std::variant<Args...> &rhs ) noexcept(
	  std::is_nothrow_move_assignable_v<std::variant<Args...>> ) {
		auto tmp = daw::move( lhs );
		lhs = daw::exchange( rhs, daw::move( tmp ) );
	}

	template<typename... Args>
	constexpr void
	cswap( std::basic_string_view<Args...> &lhs,
	       std::basic_string_view<Args...>
	         &rhs ) noexcept( std::
	                            is_nothrow_move_assignable_v<
	                              std::basic_string_view<Args...>> ) {
		auto tmp = daw::move( lhs );
		lhs = daw::exchange( rhs, daw::move( tmp ) );
	}

	template<typename T>
	constexpr void cswap( std::optional<T> &lhs, std::optional<T> &rhs ) noexcept(
	  std::is_nothrow_move_assignable_v<std::optional<T>> ) {
		auto tmp = daw::move( lhs );
		lhs = daw::exchange( rhs, daw::move( tmp ) );
	}

	template<typename T>
	constexpr void cswap( T &&lhs, T &&rhs ) noexcept(
	  std::conjunction_v<std::is_nothrow_move_assignable<T>,
	                     std::is_nothrow_move_constructible<T>> ) {
		static_assert( !std::is_const_v<std::remove_reference_t<T>>,
		               "Cannot swap const values" );
		if constexpr( std::is_trivial_v<std::remove_reference_t<T>> ) {
			auto tmp = lhs;
			lhs = daw::exchange( rhs, tmp );
		} else if constexpr( std::is_scalar_v<std::remove_reference_t<T>> ) {
			auto tmp = daw::move( lhs );
			lhs = daw::exchange( daw::move( rhs ), daw::move( tmp ) );
		} else if constexpr( swap_details::has_member_swap_v<
		                       std::remove_reference_t<T>> ) {
			lhs.swap( rhs );
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
		auto tmp = std::move( *lhs );
		*lhs = std::move( *rhs );
		*rhs = std::move( tmp );
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

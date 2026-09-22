// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_consteval.h"

#include <concepts>
#include <iterator>
#include <type_traits>

namespace daw {
	template<typename T>
	DAW_CONSTEVAL bool check_movable( ) {
		static_assert( std::is_object_v<T> );
		static_assert( std::move_constructible<T> );
		static_assert( std::assignable_from<T &, T> );
		static_assert( std::swappable<T> );
		return true;
	}

	template<typename I>
	DAW_CONSTEVAL bool check_weakly_incrementable( ) {
		check_movable<I>( );
		static_assert( requires( I i ) { typename std::iter_difference_t<I>; } );
		static_assert( requires { typename std::iter_difference_t<I>; } );
		static_assert( daw::is_integral_v<std::iter_difference_t<I>> );
		static_assert( daw::is_signed_v<std::iter_difference_t<I>> );
		static_assert( requires( I i ) {
			{ ++i } -> std::same_as<I &>;
		} );
		static_assert( requires( I i ) { i++; } );
		return true;
	}

	template<typename I>
	DAW_CONSTEVAL bool check_input_or_output_iterator( ) {
		static_assert( requires( I i ) { *i; } );
		static_assert( requires( I i ) {
			{ *i } -> not_same_as<void>;
		} );
		check_weakly_incrementable<I>( );
		return true;
	}

	template<typename I>
	DAW_CONSTEVAL bool check_indirectly_readable( ) {
		static_assert( requires { typename std::iter_value_t<I>; } );
		static_assert( requires { typename std::iter_reference_t<I>; } );
		static_assert( requires { typename std::iter_rvalue_reference_t<I>; } );
		static_assert( requires( I const in ) {
			{ *in } -> std::same_as<std::iter_reference_t<I>>;
		} );
		static_assert( requires( I const in ) {
			{
				std::ranges::iter_move( in )
			} -> std::same_as<std::iter_rvalue_reference_t<I>>;
		} );
		static_assert( std::common_reference_with<std::iter_reference_t<I> &&,
		                                          std::iter_value_t<I> &> );
		static_assert(
		  std::common_reference_with<std::iter_reference_t<I> &&,
		                             std::iter_rvalue_reference_t<I> &&> );
		static_assert(
		  std::common_reference_with<std::iter_rvalue_reference_t<I> &&,
		                             const std::iter_value_t<I> &> );
		return true;
	}

	template<typename I>
	DAW_CONSTEVAL bool check_input_iterator( ) {
		check_input_or_output_iterator<I>( );
		check_indirectly_readable<I>( );
		//				requires { typename /*ITER_CONCEPT*/<I>; } &&
		//				std::derived_from</*ITER_CONCEPT*/<I>, std::input_iterator_tag>;
		return true;
	}

	template<typename T>
	DAW_CONSTEVAL bool check_semi_regular( ) {
		static_assert( std::copyable<T> );
		static_assert( std::default_initializable<T> );
		return true;
	}

	template<typename T, typename U>
	DAW_CONSTEVAL bool check_weakly_equality_comparible_with( ) {
		static_assert( requires( std::remove_reference_t<T> const &t,
		                         std::remove_reference_t<U> const &u ) {
			{ t == u } -> convertible_to<bool>;
		} );
		static_assert( requires( std::remove_reference_t<T> const &t,
		                         std::remove_reference_t<U> const &u ) {
			{ t != u } -> convertible_to<bool>;
		} );
		static_assert( requires( std::remove_reference_t<T> const &t,
		                         std::remove_reference_t<U> const &u ) {
			{ u == t } -> convertible_to<bool>;
		} );
		static_assert( requires( std::remove_reference_t<T> const &t,
		                         std::remove_reference_t<U> const &u ) {
			{ u != t } -> convertible_to<bool>;
		} );
		return true;
	}

	template<typename Sentinel, typename It>
	DAW_CONSTEVAL bool check_sentinel_for( ) {
		static_assert( check_semi_regular<Sentinel>( ) );
		static_assert( check_input_or_output_iterator<It>( ) );
		static_assert(
		  daw::check_weakly_equality_comparible_with<Sentinel, It>( ) );
		return true;
	}

	template<typename R>
	DAW_CONSTEVAL bool check_input_range( ) {
		static_assert( std::ranges::range<R> );
		static_assert( std::input_iterator<std::ranges::iterator_t<R>> );
		return true;
	}

	template<typename R>
	DAW_CONSTEVAL bool check_forward_range( ) {
		static_assert( check_input_range<R>( ) );
		static_assert( std::forward_iterator<std::ranges::iterator_t<R>> );
		return true;
	}

	template<typename R>
	DAW_CONSTEVAL bool check_bidirectional_range( ) {
		static_assert( check_forward_range<R>( ) );
		static_assert( std::bidirectional_iterator<std::ranges::iterator_t<R>> );
		return true;
	}

	template<typename R>
	DAW_CONSTEVAL bool check_random_access_range( ) {
		static_assert( check_bidirectional_range<R>( ) );
		static_assert( std::random_access_iterator<std::ranges::iterator_t<R>> );
		return true;
	}
} // namespace daw

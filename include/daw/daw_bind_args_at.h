// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_17.h"
#include "daw/daw_is_detected.h"
#include "daw/daw_cpp20_concept.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"

#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace daw {
	template<
	  std::size_t StartN, typename... Args, std::size_t... Is,
	  std::enable_if_t<( sizeof...( Args ) >= ( sizeof...( Is ) + StartN ) ),
	                   std::nullptr_t> = nullptr>
	constexpr auto split_args_impl( std::tuple<Args...> &&args,
	                                std::index_sequence<Is...> ) {
		if constexpr( sizeof...( Args ) == 0 ) {
			return std::tuple<>{ };
		} else {
			return std::tuple<std::decay_t<decltype( std::get<StartN + Is>( std::move(
			  args ) ) )>...>( std::get<StartN + Is>( std::move( args ) )... );
		}
	}

	template<
	  std::size_t StartN, std::size_t EndN, typename... Args,
	  std::enable_if_t<( sizeof...( Args ) >= EndN ), std::nullptr_t> = nullptr>
	constexpr auto split_args( std::tuple<Args...> args ) {
		return split_args_impl<StartN>(
		  std::move( args ), std::make_index_sequence<EndN - StartN>{ } );
	}

	template<typename Func>
	struct applier_t {
		Func f;
		template<typename... Args,
		         std::enable_if_t<std::is_invocable_v<Func, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto )
		operator( )( std::tuple<Args...> const &args ) const
		  noexcept( noexcept( std::apply( std::declval<Func>( ), args ) ) ) {
			return std::apply( f, args );
		}

		template<typename... Args,
		         std::enable_if_t<std::is_invocable_v<Func, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) operator( )( std::tuple<Args...> &&args ) const
		  noexcept( noexcept( std::apply( std::declval<Func>( ),
		                                  std::move( args ) ) ) ) {
			return std::apply( f, std::move( args ) );
		}
	};

	template<size_t N, typename TpArgs, typename... Args>
	using enough_args_t = std::bool_constant<(
	  (sizeof...( Args ) + std::tuple_size_v<TpArgs>) >= N )>;

	template<typename N, typename Invokable, typename TpArgs, typename... Args>
	using can_call_test = std::is_invocable<
	  applier_t<Invokable>,
	  decltype( std::tuple_cat(
	    split_args<0, N::value>(
	      std::forward_as_tuple( DAW_FWD( std::declval<Args>( ) )... ) ),
	    std::declval<TpArgs>( ),
	    split_args<N::value, sizeof...( Args )>(
	      std::forward_as_tuple( DAW_FWD( std::declval<Args>( ) )... ) ) ) )>;

namespace bind_args_impl {
	template<size_t N, typename Invokable, typename TpArgs, typename... Args>
	DAW_CPP20_CONCEPT can_call_v =
		daw::is_detected_v<can_call_test, std::integral_constant<std::size_t, N>,
											 Invokable, TpArgs, Args...>;
}

namespace bind_args_impl {
	template<size_t N, typename Invokable, typename TpArgs, typename... Args>
	DAW_CPP20_CONCEPT is_nothrow_callable_v = noexcept( std::apply(
		std::declval<Invokable>( ),
		std::tuple_cat( split_args<0, N>( std::forward_as_tuple(
											DAW_FWD( std::declval<Args>( ) )... ) ),
										std::declval<TpArgs>( ),
										split_args<N, sizeof...( Args )>( std::forward_as_tuple(
											DAW_FWD( std::declval<Args>( ) )... ) ) ) ) );
}

	template<std::size_t N, typename Invokable, typename TpArgs>
	struct bind_args_at_fn {
		Invokable func;
		TpArgs tp_args;

		template<typename... Args,
		         std::enable_if_t<can_call_v<N, Invokable, TpArgs, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) operator( )( Args &&...args ) const
		  noexcept( bind_args_impl::is_nothrow_callable_v<N, Invokable, TpArgs, Args...> ) {
			return std::apply(
			  func, std::tuple_cat(
			          split_args<0, N>( std::forward_as_tuple( DAW_FWD( args )... ) ),
			          tp_args,
			          split_args<N, sizeof...( Args )>(
			            std::forward_as_tuple( DAW_FWD( args )... ) ) ) );
		}

		template<typename... Args,
		         std::enable_if_t<can_call_v<N, Invokable, TpArgs, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) operator( )( Args &&...args ) noexcept(
		  bind_args_impl::is_nothrow_callable_v<N, Invokable, TpArgs, Args...> ) {
			static_assert( N <= sizeof...( Args ) );
			return std::apply(
			  func, std::tuple_cat(
			          split_args<0, N>( std::forward_as_tuple( DAW_FWD( args )... ) ),
			          tp_args,
			          split_args<N, sizeof...( Args )>(
			            std::forward_as_tuple( DAW_FWD( args )... ) ) ) );
		}
	};

	template<std::size_t N, typename Invokable, typename... Args>
	constexpr auto bind_args_at( Invokable &&func, Args &&...args ) {
		return bind_args_at_fn<N, daw::remove_cvref_t<Invokable>,
		                       std::tuple<std::decay_t<Args>...>>{
		  DAW_FWD( func ),
		  std::tuple<std::decay_t<Args>...>( DAW_FWD( args )... ) };
	}
} // namespace daw

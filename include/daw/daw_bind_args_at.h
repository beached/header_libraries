// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw/cpp_17.h"

#include <cstddef>
#include <type_traits>
#include <iostream>
#include <tuple>
#include <utility>

namespace daw {
	template<
	  std::size_t StartN, typename... Args, std::size_t... Is,
	  std::enable_if_t<( sizeof...( Args ) >= ( sizeof...( Is ) + StartN ) ),
	                   std::nullptr_t> = nullptr>
	constexpr auto split_args_impl( std::tuple<Args...> &&args,
	                                std::index_sequence<Is...> ) {
		if constexpr( sizeof...( Args ) == 0 ) {
			return std::tuple<>{};
		} else {
			return std::tuple<std::decay_t<decltype( std::get<StartN + Is>( std::move(
			  args ) ) )>...>( std::get<StartN + Is>( std::move( args ) )... );
		}
	}

	template<
	  std::size_t StartN, std::size_t EndN, typename... Args,
	  std::enable_if_t<( sizeof...( Args ) >= EndN ), std::nullptr_t> = nullptr>
	constexpr auto split_args( std::tuple<Args...> args ) {
		return split_args_impl<StartN>( std::move( args ),
		                                std::make_index_sequence<EndN - StartN>{} );
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
	    split_args<0, N::value>( std::forward_as_tuple(
	      std::forward<Args>( std::declval<Args>( ) )... ) ),
	    std::declval<TpArgs>( ),
	    split_args<N::value, sizeof...( Args )>( std::forward_as_tuple(
	      std::forward<Args>( std::declval<Args>( ) )... ) ) ) )>;

	template<size_t N, typename Invokable, typename TpArgs, typename... Args>
	inline constexpr bool can_call_v =
	  std::experimental::is_detected_v<can_call_test,
	                                   std::integral_constant<std::size_t, N>,
	                                   Invokable, TpArgs, Args...>;
	/*
	    std::conjunction_v<enough_args_t<N, TpArgs, Args...>,
	                       can_call2<N, Invokable, TpArgs, Args...>>;
	*/
	template<size_t N, typename Invokable, typename TpArgs, typename... Args>
	inline constexpr bool is_nothrow_callable_v = noexcept( std::apply(
	  std::declval<Invokable>( ),
	  std::tuple_cat( split_args<0, N>( std::forward_as_tuple(
	                    std::forward<Args>( std::declval<Args>( ) )... ) ),
	                  std::declval<TpArgs>( ),
	                  split_args<N, sizeof...( Args )>( std::forward_as_tuple(
	                    std::forward<Args>( std::declval<Args>( ) )... ) ) ) ) );

	template<std::size_t N, typename Invokable, typename TpArgs>
	struct bind_args_at_fn {
		Invokable func;
		TpArgs tp_args;

		template<typename... Args,
		         std::enable_if_t<can_call_v<N, Invokable, TpArgs, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) operator( )( Args &&... args ) const
		  noexcept( is_nothrow_callable_v<N, Invokable, TpArgs, Args...> ) {
			return std::apply(
			  func,
			  std::tuple_cat( split_args<0, N>( std::forward_as_tuple(
			                    std::forward<decltype( args )>( args )... ) ),
			                  tp_args,
			                  split_args<N, sizeof...( Args )>( std::forward_as_tuple(
			                    std::forward<decltype( args )>( args )... ) ) ) );
		}

		template<typename... Args,
		         std::enable_if_t<can_call_v<N, Invokable, TpArgs, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) operator( )( Args &&... args ) noexcept(
		  is_nothrow_callable_v<N, Invokable, TpArgs, Args...> ) {
			return std::apply(
			  func,
			  std::tuple_cat( split_args<0, N>( std::forward_as_tuple(
			                    std::forward<decltype( args )>( args )... ) ),
			                  tp_args,
			                  split_args<N, sizeof...( Args )>( std::forward_as_tuple(
			                    std::forward<decltype( args )>( args )... ) ) ) );
		}
	};

	template<std::size_t N, typename Invokable, typename... Args>
	constexpr auto bind_args_at( Invokable &&func, Args &&... args ) {
		return bind_args_at_fn<N, remove_cvref_t<Invokable>,
		                       std::tuple<std::decay_t<Args>...>>{
		  std::forward<Invokable>( func ),
		  std::tuple<std::decay_t<Args>...>( std::forward<Args>( args )... )};
	}
}


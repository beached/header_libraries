// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <array>
#include <ciso646>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <variant>

namespace daw {
	namespace function_table_impl {
		template<
		  size_t I, typename... TpArgs,
		  std::enable_if_t<( I < sizeof...( TpArgs ) ), std::nullptr_t> = nullptr>
		constexpr decltype( auto ) sfinae_get( std::tuple<TpArgs...> const &tp ) {
			return std::get<I>( tp );
		}

		template<
		  size_t I, typename... TpArgs,
		  std::enable_if_t<( I < sizeof...( TpArgs ) ), std::nullptr_t> = nullptr>
		constexpr decltype( auto ) sfinae_get( std::tuple<TpArgs...> &&tp ) {
			return std::get<I>( DAW_MOVE( tp ) );
		}

		template<
		  size_t I, typename... TpArgs,
		  std::enable_if_t<( I < sizeof...( TpArgs ) ), std::nullptr_t> = nullptr>
		constexpr decltype( auto ) sfinae_get( std::tuple<TpArgs...> &tp ) {
			return std::get<I>( tp );
		}

		template<
		  size_t I, typename T,
		  typename F = std::integral_constant<
		    bool, ( I < std::tuple_size<std::remove_reference_t<T>>::value )>>
		struct is_t_in_range : F {};

		template<size_t I, typename T>
		constexpr bool is_t_in_range_v = is_t_in_range<I, T>::value;

		template<typename I, typename T, typename... Args>
		using is_t_callable_test = decltype( sfinae_get<I::value>(
		  std::declval<T>( ) )( std::declval<Args>( )... ) );

		template<size_t I, typename T, typename... Args>
		constexpr bool is_t_callable_v =
		  is_detected_v<is_t_callable_test, std::integral_constant<size_t, I>, T,
		                Args...>;

		template<typename R, bool allow_empty, size_t I, typename T,
		         typename... Args>
		static constexpr R apply_at_impl( size_t idx, T &&t, Args &&...args ) {
			if( idx == I ) {
				if constexpr( is_t_callable_v<I, T, Args...> ) {
					return sfinae_get<I>( std::forward<T>( t ) )(
					  std::forward<Args>( args )... );
				} else if constexpr( allow_empty and is_t_callable_v<I, T> ) {
					return sfinae_get<I>( std::forward<T>( t ) )( );
				} else {
					std::abort( );
				}
			}
			if constexpr( is_t_in_range_v<I + 1, T> ) {
				return apply_at_impl<R, allow_empty, I + 1>(
				  idx, std::forward<T>( t ), std::forward<Args>( args )... );
			}
			std::abort( );
		}

		template<size_t I, typename T, typename Value>
		static constexpr void set_at_impl( size_t idx, T &&t, Value &&v ) {
			if( idx == I ) {
				if constexpr( std::is_assignable_v<decltype( std::get<I>( t ) ),
				                                   Value> ) {
					std::get<I>( t ) = std::forward<Value>( v );
					return;
				} else {
					std::abort( );
				}
			}
			if constexpr( is_t_in_range_v<I + 1, T> ) {
				set_at_impl<I + 1>( idx, std::forward<T>( t ),
				                    std::forward<Value>( v ) );
				return;
			}
			std::abort( );
		} // namespace
	}   // namespace function_table_impl

	template<typename R, typename Function, typename... Functions>
	struct function_table_t {
		static constexpr bool using_array_v =
		  daw::traits::all_same_v<Function, Functions...>;
		using value_t =
		  std::conditional_t<using_array_v,
		                     std::array<Function, sizeof...( Functions ) + 1U>,
		                     std::tuple<Function, Functions...>>;

		value_t fns{ };

		constexpr function_table_t( ) = default;
		constexpr function_table_t( Function f, Functions... fs )
		  : fns{ f, fs... } {}

		template<bool allow_empty = false, typename... Args>
		constexpr R call( size_t idx, Args &&...args ) const {
			if constexpr( using_array_v ) {
				return fns[idx]( std::forward<Args>( args )... );
			} else {
				return function_table_impl::apply_at_impl<R, allow_empty, 0>(
				  idx, fns, std::forward<Args>( args )... );
			}
		}

		template<bool allow_empty = false, typename... Args>
		constexpr R operator( )( size_t idx, Args &&...args ) const {
			return call( idx, std::forward<Args>( args )... );
		}

		template<typename Value>
		constexpr void set( size_t idx, Value &&v ) {
			if constexpr( using_array_v ) {
				fns[idx] = std::forward<Value>( v );
			} else {
				function_table_impl::set_at_impl<0>( idx, fns,
				                                     std::forward<Value>( v ) );
			}
		}

		static constexpr size_t size( ) noexcept {
			return sizeof...( Functions ) + 1U;
		}
	};

	template<typename R = void, typename Function, typename... Functions>
	[[nodiscard]] constexpr function_table_t<R, Function, Functions...>
	make_function_table( Function fn, Functions... fns ) {
		return function_table_t<R, Function, Functions...>{ fn, fns... };
	}
} // namespace daw

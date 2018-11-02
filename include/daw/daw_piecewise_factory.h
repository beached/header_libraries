// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <optional>
#include <tuple>
#include <variant>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace impl {
		template<typename... Ts>
		struct overloaded : Ts... {
			using Ts::operator( )...;
		};

		template<typename... Ts>
		overloaded( Ts... )->overloaded<Ts...>;

		template<typename F, typename Tuple, std::size_t... I>
		constexpr decltype( auto )
		piecewise_applier_impl( F &&f, Tuple &&t, std::index_sequence<I...> ) {
			return daw::invoke( std::forward<F>( f ),
			                    ( *( std::get<I>( std::forward<Tuple>( t ) ) ) )... );
		}

		template<size_t N, size_t ArgSize, typename Tuple, typename Value,
		         std::enable_if_t<( N >= ArgSize ), std::nullptr_t> = nullptr>
		constexpr void set_tuple( size_t, Tuple &, Value && ) {
			daw::exception::daw_throw<std::out_of_range>( );
		}

		template<template<class> class T, class Arg>
		constexpr Arg remove_layer_func(T<Arg>);

		template<size_t N, size_t ArgSize, typename Tuple, typename Value,
		         std::enable_if_t<( N < ArgSize ), std::nullptr_t> = nullptr>
		constexpr void set_tuple( size_t index, Tuple &tp, Value &&value ) {
			if( N != index ) {
				set_tuple<N + 1, ArgSize>( index, tp, std::forward<Value>( value ) );
				return;
			}
			using value_t = decltype(remove_layer_func(
                    std::declval<std::tuple_element_t<N, Tuple>>()) );

			auto const setter =
			  overloaded{[&]( value_t const &v ) { std::get<N>( tp ) = v; },
			             [&]( value_t &&v ) { std::get<N>( tp ) = std::move( v ); },
			             []( ... ) noexcept {}};

			setter( std::forward<Value>( value ) );
		}
	} // namespace impl

	template<typename F, typename... Args>
	constexpr decltype( auto ) piecewise_applier( F &&f,
	                                              std::tuple<Args...> const &t ) {
		return impl::piecewise_applier_impl(
		  std::forward<F>( f ), t, std::make_index_sequence<sizeof...( Args )>{} );
	}

	template<typename F, typename... Args>
	constexpr decltype( auto ) piecewise_applier( F &&f,
	                                              std::tuple<Args...> &&t ) {
		return impl::piecewise_applier_impl(
		  std::forward<F>( f ), std::move( t ),
		  std::make_index_sequence<sizeof...( Args )>{} );
	}

	template<typename T, typename... Args>
	class piecewise_factory_t {
		std::tuple<std::optional<Args>...> m_args;

	public:
		constexpr T
		operator( )( ) const &noexcept( is_nothrow_constructible_v<T, Args...> ) {
			return piecewise_applier(
			  []( Args const &... args ) { return construct_a<T>{}( args... ); },
			  m_args );
		}

		constexpr T operator( )( ) &
		  noexcept( is_nothrow_constructible_v<T, Args...> ) {
			return piecewise_applier(
			  []( Args const &... args ) { return construct_a<T>{}( args... ); },
			  m_args );
		}

		constexpr T operator( )( ) &&
		  noexcept( is_nothrow_constructible_v<T, Args...> ) {
			return piecewise_applier(
			  []( Args &&... args ) {
				  return construct_a<T>{}( std::move( args )... );
			  },
			  std::move( m_args ) );
		}

		template<size_t N>
		constexpr void
		set( typename std::tuple_element<N, std::tuple<Args...>>::type const
		       &arg ) noexcept( is_nothrow_copy_constructible_v<decltype( arg )> ) {
			std::get<N>( m_args ) = arg;
		}

		template<size_t N>
		constexpr void
		set( typename std::tuple_element<N, std::tuple<Args...>>::type &&
		       arg ) noexcept( is_nothrow_move_constructible_v<decltype( arg )> ) {
			std::get<N>( m_args ) = std::move( arg );
		}

		template<typename Value>
		constexpr void set( size_t index, Value &&value ) {
			impl::set_tuple<0, sizeof...( Args )>( index, m_args,
			                                       std::forward<Value>( value ) );
		}
	};
} // namespace daw

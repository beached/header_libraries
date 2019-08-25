// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

namespace daw {
	namespace visit_impl {
		namespace {
			// A simple overload class that is constructed via aggregate.
			template<typename... Fs>
			struct overload_t : Fs... {
				using Fs::operator( )...;
			};
			template<typename... Fs>
			overload_t( Fs... )->overload_t<Fs...>;

			template<typename F, typename... Fs>
			constexpr decltype( auto ) overload( F &&f, Fs &&... fs ) {
				if constexpr( sizeof...( Fs ) > 0 ) {
					return overload_t{std::forward<F>( f ), std::forward<Fs>( fs )...};
				} else {
					return std::forward<F>( f );
				}
			}

			template<size_t N, typename R, typename Variant, typename Visitor>
			[[nodiscard]] constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
				using std::get;
				if constexpr( N == 0 ) {
					// If this check isnt there the compiler will generate
					// exception code, this stops that
					if( N != var.index( ) ) {
						std::abort( );
					}
					return std::forward<Visitor>( vis )(
					  get<N>( std::forward<Variant>( var ) ) );
				} else {
					if( var.index( ) == N ) {
						if constexpr( std::is_invocable_v<Visitor,
						                                  decltype( get<N>( var ) )> ) {

							// If this check isnt there the compiler will generate
							// exception code, this stops that
							return std::forward<Visitor>( vis )(
							  get<N>( std::forward<Variant>( var ) ) );
						} else {
							std::abort( );
						}
					}
					return visit_nt<N - 1, R>( std::forward<Variant>( var ),
					                           std::forward<Visitor>( vis ) );
				}
			}
		} // namespace
	}   // namespace visit_impl
	//**********************************************

	// Singe visitation visit.  Expects that const lvalue variant is valid and not
	// empty
	template<template<class...> class Variant, typename... Args,
	         typename... Visitors>
	[[nodiscard]] constexpr decltype( auto )
	visit_nt( Variant<Args...> const &var, Visitors &&... visitors ) {

		static_assert( sizeof...( Args ) > 0 );
		using std::get;
		using result_t = decltype( visit_impl::overload(
		  std::forward<Visitors>( visitors )... )( get<0>( var ) ) );

		return visit_impl::visit_nt<sizeof...( Args ) - 1, result_t>(
		  var, visit_impl::overload( std::forward<Visitors>( visitors )... ) );
	}

	// Singe visitation visit.  Expects that lvalue variant is valid and not empty
	template<template<class...> class Variant, typename... Args,
	         typename... Visitors>
	[[nodiscard]] constexpr decltype( auto ) visit_nt( Variant<Args...> &var,
	                                                   Visitors &&... visitors ) {

		static_assert( sizeof...( Args ) > 0 );
		using std::get;
		using result_t = decltype( visit_impl::overload(
		  std::forward<Visitors>( visitors )... )( get<0>( var ) ) );

		return visit_impl::visit_nt<sizeof...( Args ) - 1, result_t>(
		  var, visit_impl::overload( std::forward<Visitors>( visitors )... ) );
	}

	// Singe visitation visit.  Expects that rvalue variant is valid and not empty
	template<template<class...> class Variant, typename... Args,
	         typename... Visitors>
	[[nodiscard]] constexpr decltype( auto ) visit_nt( Variant<Args...> &&var,
	                                                   Visitors &&... visitors ) {

		static_assert( sizeof...( Args ) > 0 );
		using std::get;
		using result_t = decltype( visit_impl::overload(
		  std::forward<Visitors>( visitors )... )( get<0>( daw::move( var ) ) ) );

		return visit_impl::visit_nt<sizeof...( Args ) - 1, result_t>(
		  daw::move( var ),
		  visit_impl::overload( std::forward<Visitors>( visitors )... ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool
	  is_visitable_v = ( std::is_invocable_v<Visitors, Value> or ... );

} // namespace daw

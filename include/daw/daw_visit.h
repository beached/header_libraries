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
			template<typename... Fs>
			struct overload : Fs... {
				using Fs::operator( )...;
			};

			template<typename... Fs>
			overload( Fs... )->overload<Fs...>;

			template<size_t N, typename R, typename Variant, typename Visitor>
			[[nodiscard]] constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
				if constexpr( N == 0 ) {
					if( N != var.index( ) ) {
						std::abort( );
					}
					// If this check isnt there the compiler will generate
					// exception code, this stops that
					return std::forward<Visitor>( vis )(
					  std::get<N>( std::forward<Variant>( var ) ) );
				} else {
					if( var.index( ) == N ) {
						if constexpr( std::is_invocable_v<Visitor, decltype( std::get<N>(
						                                             var ) )> ) {
							return std::forward<Visitor>( vis )(
							  std::get<N>( std::forward<Variant>( var ) ) );
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

	template<class... Args, typename Visitor, typename... Visitors>
	[[nodiscard]] constexpr decltype( auto )
	visit_nt( std::variant<Args...> const &var, Visitor &&vis,
	          Visitors &&... visitors ) {
		if constexpr( sizeof...( Visitors ) == 0 ) {
			using result_t = decltype(
			  std::invoke( std::forward<Visitor>( vis ), std::get<0>( var ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>(
			  var, std::forward<Visitor>( vis ) );

		} else {
			auto ol = visit_impl::overload{std::forward<Visitor>( vis ),
			                               std::forward<Visitors>( visitors )...};
			using result_t =
			  decltype( std::invoke( std::move( ol ), std::get<0>( var ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>( var, std::move( ol ) );
		}
	}

	template<class... Args, typename Visitor, typename... Visitors>
	[[nodiscard]] constexpr decltype( auto ) visit_nt( std::variant<Args...> &var,
	                                                   Visitor &&vis,
	                                                   Visitors &&... visitors ) {
		if constexpr( sizeof...( Visitors ) == 0 ) {
			using result_t = decltype(
			  std::invoke( std::forward<Visitor>( vis ), std::get<0>( var ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>(
			  var, std::forward<Visitor>( vis ) );
		} else {
			auto ol = visit_impl::overload{std::forward<Visitor>( vis ),
			                               std::forward<Visitors>( visitors )...};
			using result_t =
			  decltype( std::invoke( std::move( ol ), std::get<0>( var ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>( var, std::move( ol ) );
		}
	}

	template<class... Args, typename Visitor, typename... Visitors>
	[[nodiscard]] constexpr decltype( auto )
	visit_nt( std::variant<Args...> &&var, Visitor &&vis,
	          Visitors &&... visitors ) {
		if constexpr( sizeof...( Visitors ) == 0 ) {
			using result_t = decltype( std::invoke(
			  std::forward<Visitor>( vis ), std::move( std::get<0>( var ) ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>(
			  std::move( var ), std::forward<Visitor>( vis ) );

		} else {
			auto ol = visit_impl::overload{std::forward<Visitor>( vis ),
			                               std::forward<Visitors>( visitors )...};
			using result_t = decltype(
			  std::invoke( std::move( ol ), std::move( std::get<0>( var ) ) ) );

			static_assert( sizeof...( Args ) > 0 );
			return visit_nt<sizeof...( Args ) - 1, result_t>( std::move( var ),
			                                                  std::move( ol ) );
		}
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool
	  is_visitable_v = ( std::is_invocable_v<Visitors, Value> or ... );

} // namespace daw


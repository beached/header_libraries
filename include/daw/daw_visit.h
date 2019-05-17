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

#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

#include "cpp_17.h"
#include "daw_move.h"
#include "daw_overload.h"

namespace daw {
	template<size_t, size_t, typename R, typename... Args>
	[[noreturn]] constexpr R visit_nt( Args &&... ) noexcept {
		// This will never be called, it is there to ensure the non-called path will
		// look like it can be called
		std::terminate( );
	}

	template<size_t N, size_t MaxN, typename R, typename Variant,
	         typename Visitor,
	         std::enable_if_t<( N < MaxN ), std::nullptr_t> = nullptr>
	constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
		if( var.index( ) == N ) {
			if constexpr( daw::is_reference_wrapper_v<decltype(
			                std::get<N>( std::forward<Variant>( var ) ) )> ) {
				return std::invoke(
				  std::forward<Visitor>( vis ),
				  std::get<N>( std::forward<Variant>( var ) ).get( ) );
			} else {
				return std::invoke( std::forward<Visitor>( vis ),
				                    std::get<N>( std::forward<Variant>( var ) ) );
			}
		}
		return visit_nt<N + 1, MaxN, R>( std::forward<Variant>( var ),
		                                 std::forward<Visitor>( vis ) );
	}

	template<class... Args, typename Visitor, typename... Visitors>
	constexpr decltype( auto ) visit_nt( std::variant<Args...> const &var,
	                                     Visitor &&vis,
	                                     Visitors &&... visitors ) {
		auto ol = daw::overload( std::forward<Visitor>( vis ),
		                         std::forward<Visitors>( visitors )... );
		using result_t =
		  decltype( std::invoke( daw::move( ol ), std::get<0>( var ) ) );

		if( var.index( ) == 0 ) {
			return std::invoke( daw::move( ol ), std::get<0>( var ) );
		}
		return visit_nt<1, sizeof...( Args ), result_t>( var, daw::move( ol ) );
	}

	template<class... Args, typename Visitor, typename... Visitors>
	constexpr decltype( auto ) visit_nt( std::variant<Args...> &var,
	                                     Visitor &&vis,
	                                     Visitors &&... visitors ) {
		auto ol = daw::overload( std::forward<Visitor>( vis ),
		                         std::forward<Visitors>( visitors )... );
		using result_t =
		  decltype( std::invoke( daw::move( ol ), std::get<0>( var ) ) );

		if( var.index( ) == 0 ) {
			return std::invoke( daw::move( ol ), std::get<0>( var ) );
		}
		return visit_nt<1, sizeof...( Args ), result_t>( var, daw::move( ol ) );
	}

	template<class... Args, typename Visitor, typename... Visitors>
	constexpr decltype( auto ) visit_nt( std::variant<Args...> &&var,
	                                     Visitor &&vis,
	                                     Visitors &&... visitors ) {
		auto ol = daw::overload( std::forward<Visitor>( vis ),
		                         std::forward<Visitors>( visitors )... );
		using result_t = decltype(
		  std::invoke( daw::move( ol ), daw::move( std::get<0>( var ) ) ) );

		if( var.index( ) == 0 ) {
			return std::invoke( daw::move( ol ), std::get<0>( daw::move( var ) ) );
		}
		return visit_nt<1, sizeof...( Args ), result_t>( daw::move( var ),
		                                                 daw::move( ol ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool
	  is_visitable_v = ( std::is_invocable_v<Visitors, Value> or ... );
} // namespace daw

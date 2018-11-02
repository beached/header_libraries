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

#include "cpp_17.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace impl {
		template<typename F, typename Tuple, std::size_t... I>
		constexpr decltype( auto ) piecewise_applier_impl( F &&f, Tuple &&t,
		                                       std::index_sequence<I...> ) {
			return daw::invoke( std::forward<F>( f ),
			                    ( *( std::get<I>( std::forward<Tuple>( t ) ) ) )... );
		}
	} // namespace impl

	template<typename F, typename...Args>
	constexpr decltype( auto ) piecewise_applier( F &&f, std::tuple<Args...> const &t ) {
		return impl::piecewise_applier_impl(
		  std::forward<F>( f ), t, 
		  std::make_index_sequence<sizeof...(Args)>{} );
	}

	template<typename F, typename...Args>
	constexpr decltype( auto ) piecewise_applier( F &&f, std::tuple<Args...> &&t ) {
		return impl::piecewise_applier_impl(
		  std::forward<F>( f ), std::move( t ), 
		  std::make_index_sequence<sizeof...(Args)>{} );
	}

	template<typename T, typename... Args>
	class piecewise_factory_t {
		std::tuple<std::optional<Args>...> m_args;
	public:
		constexpr T
		operator( )( ) const noexcept( is_nothrow_constructible_v<T, Args...> ) {
			return piecewise_applier(
			  []( Args const &... args ) {
				  //return construct_a<T>( args... );
					return T{ args... };
			  },
			  m_args );
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
	};
} // namespace daw


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

#include <cstddef>
#include <utility>

#include "daw_traits.h"

namespace daw {
	// overload_t/overload create a callable with the overloads of operator( )
	// provided
	//
	template<typename... Functions>
	struct overload_t;

	template<typename Function>
	struct overload_t<Function> {
	private:
		mutable Function m_func;

	public:
		template<typename Func, std::enable_if_t<daw::is_same_v<Func, Function>,
		                                         std::nullptr_t> = nullptr>
		constexpr overload_t( Func &&func )
		  : m_func( std::forward<Func>( func ) ) {}

		template<typename... Args,
		         std::enable_if_t<traits::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr auto operator( )( Args &&... args ) const noexcept(
		  noexcept( std::declval<Function>( )( std::declval<Args>( )... ) ) )
		  -> decltype( std::declval<Function>( )( std::declval<Args>( )... ) ) {

			return m_func( std::forward<Args>( args )... );
		}

		template<typename... Args,
		         std::enable_if_t<traits::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr auto operator( )( Args &&... args ) noexcept(
		  noexcept( std::declval<Function>( )( std::declval<Args>( )... ) ) )
		  -> decltype( std::declval<Function>( )( std::declval<Args>( )... ) ) {

			return m_func( std::forward<Args>( args )... );
		}
	};

	template<typename Function, typename... Functions>
	struct overload_t<Function, Functions...> : public overload_t<Function>,
	                                            public overload_t<Functions...> {
		using overload_t<Function>::operator( );
		using overload_t<Functions...>::operator( );

		template<
		  typename Func, typename... Funcs,
		  std::enable_if_t<!daw::is_function_v<Func>, std::nullptr_t> = nullptr>
		constexpr overload_t( Func &&func, Funcs &&... funcs )
		  : overload_t<Func>( std::forward<Func>( func ) )
		  , overload_t<Funcs...>( std::forward<Funcs>( funcs )... ) {}
	};

	template<typename... Functions>
	constexpr auto overload( Functions &&... funcs ) {
		return overload_t<Functions...>{std::forward<Functions>( funcs )...};
	}

	template<typename... Args, typename OverloadSet,
	         std::enable_if_t<traits::is_callable_v<OverloadSet, Args...>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) empty_overload( OverloadSet &&overload_set ) {
		return std::forward<OverloadSet>( overload_set );
	}

	template<typename... Args, typename OverloadSet,
	         std::enable_if_t<!traits::is_callable_v<OverloadSet, Args...>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) empty_overload( OverloadSet &&overload_set ) {
		return overload( std::forward<OverloadSet>( overload_set ),
		                 []( Args && ... ) mutable noexcept {} );
	}
} // namespace daw

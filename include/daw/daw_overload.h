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

namespace daw {
	// overload_t/overload create a callable with the overloads of operator( )
	// provided
	template<class... Ts>
	struct overloaded_t {};

	template<class T0>
	struct overloaded_t<T0> : T0 {
		using T0::operator( );

		constexpr overloaded_t( T0 &&t0 )
		  : T0( std::forward<T0>( t0 ) ) {}
	};

	template<class T0, class T1, class... Ts>
	struct overloaded_t<T0, T1, Ts...> : T0, overloaded_t<T1, Ts...> {
		using T0::operator( );
		using overloaded_t<T1, Ts...>::operator( );

		constexpr overloaded_t( T0 &&t0, T1 &&t1, Ts &&... ts )
		  : T0( std::move( t0 ) )
		  , overloaded_t<T1, Ts...>( std::forward<T1>( t1 ),
		                             std::forward<Ts>( ts )... ) {}
	};

	template<class... Ts>
	constexpr overloaded_t<std::decay_t<Ts>...> overload( Ts &&... ts ) {
		return {std::forward<Ts>( ts )...};
	}

	// creates an overload set but adds a default noop.  only works for void
	template<typename... Args, typename OverloadSet,
	         std::enable_if_t<daw::is_callable_v<OverloadSet, Args...>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) empty_overload( OverloadSet &&overload_set ) {
		return std::forward<OverloadSet>( overload_set );
	}

	template<typename... Args, typename OverloadSet,
	         std::enable_if_t<!daw::is_callable_v<OverloadSet, Args...>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) empty_overload( OverloadSet &&overload_set ) {
		return overload( std::forward<OverloadSet>( overload_set ),
		                 []( Args && ... ) mutable noexcept {} );
	}
} // namespace daw


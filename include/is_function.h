// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <type_traits>

namespace daw {

	template<int I>
	struct priority_tag : priority_tag<I - 1> {};

	template<>
	struct priority_tag<0> {};

	namespace impl {
		// Function types:
		// template<typename T>
		constexpr bool is_function_impl( priority_tag<0> ) {
			return true;
		}

		// // Array types:
		// template<typename T, typename = decltype( std::declval<T&>( )[0] )>
		// constexpr bool is_function_impl_( priority_tag<1> ) { return false; }
		//
		// // Anything that can be returned from a function ( including
		// // void, reference, and scalar types ):
		template<typename T, typename = T ( * )( )>
		constexpr bool is_function_impl( priority_tag<2> ) {
			return false;
		}
		//
		// // Classes ( notably abstract class types that would not be caught by the above ):
		template<typename T, typename = int T::*>
		constexpr bool is_function_impl( priority_tag<3> ) {
			return false;
		}
	} // namespace impl

	template<typename T>
	constexpr bool is_function_v = impl::is_function_impl<T>( priority_tag<3>{} );

	template<typename T>
	struct is_function : std::bool_constant<is_function_v<T>> {};
} // namespace daw

// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include "cpp_17.h"

namespace daw {
	namespace algorithm {
		namespace algorithm_impl {
#ifdef _MSC_VER
			template<typename F, typename... Args>
			constexpr void invoker( F &&func, Args &&... args ) noexcept(
			  noexcept( std::forward<F>( func )( std::forward<Args>( args )... ) ) ) {

				(void)std::forward<F>( func )( std::forward<Args>( args )... );
			}
#endif
			template<typename Function, typename... Args, size_t... Is>
			constexpr void do_n(
			  Function &&func, Args &&... args,
			  std::index_sequence<
			    Is...> ) noexcept( std::is_nothrow_invocable_v<Function, Args...> ) {

				if constexpr( sizeof...( Is ) > 0 ) {
#ifndef _MSC_VER
					(void)( ( daw::invoke( func, args... ), Is ) + ... );
#else
					(void)( ( invoker( func, args... ), Is ) + ... );
#endif
				}
			}

			template<typename Function, size_t... Is>
			constexpr void
			do_n_arg( Function &&func, std::index_sequence<Is...> ) noexcept(
			  std::is_nothrow_invocable_v<Function, size_t> ) {

				if constexpr( sizeof...( Is ) > 0 ) {
#ifndef _MSC_VER
					(void)( ( daw::invoke( func, Is ), 0 ) + ... );
#else
					(void)( ( invoker( func, Is ), 0 ) + ... );
#endif
				}
			}

		} // namespace algorithm_impl

		template<size_t count, typename Function, typename... Args>
		constexpr void do_n( Function &&func, Args &&... args ) noexcept(
		  std::is_nothrow_invocable_v<Function, Args...> ) {
			algorithm_impl::do_n( std::forward<Function>( func ),
			                      std::forward<Args>( args )...,
			                      std::make_index_sequence<count>{} );
		}

		template<typename Function, typename... Args>
		constexpr void
		do_n( size_t count, Function &&func, Args &&... args ) noexcept(
		  std::is_nothrow_invocable_v<Function, Args...> ) {
			while( count-- > 0 ) {
#ifndef _MSC_VER
				daw::invoke( func, args... );
#else
				invoker( func, args... );
#endif
			}
		}

		template<typename Function, typename... Args>
		constexpr void do_n_arg( size_t count, Function &&func ) noexcept(
		  std::is_nothrow_invocable_v<Function, size_t> ) {
			size_t n = 0;
			while( n < count ) {
#ifndef _MSC_VER
				daw::invoke( func, n++ );
#else
				invoker( func, n++ );
#endif
			}
		}

		template<size_t count, typename Function, typename... Args>
		constexpr void do_n_arg( Function &&func ) noexcept(
		  std::is_nothrow_invocable_v<Function, size_t> ) {
			algorithm_impl::do_n_arg( std::forward<Function>( func ),
			                          std::make_index_sequence<count>{} );
		}
	} // namespace algorithm
} // namespace daw


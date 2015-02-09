#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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


#include <memory>
#include <type_traits>
#include <utility>

namespace daw {
	template <typename T, typename... Args>
	std::unique_ptr<T> make_unique_helper( std::false_type, Args&&... args ) {
		return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
	}

	template <typename T, typename... Args>
	std::unique_ptr<T> make_unique_helper( std::true_type, Args&&... args ) {
		static_assert(std::extent<T>::value == 0, "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

		typedef typename std::remove_extent<T>::type U;
		return std::unique_ptr<T>( new U[sizeof...(Args)]{std::forward<Args>( args )...} );
	}

	template <typename T, typename... Args>
	std::unique_ptr<T> make_unique( Args&&... args ) {
		try {
			auto result = make_unique_helper<T>( std::is_array<T>( ), std::forward<Args>( args )... );
			return result;
		} catch( ... ) {
			return std::unique_ptr<T>( );
		}
	}
}	// namespace daw

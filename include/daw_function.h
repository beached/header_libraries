// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <tuple>
#include "daw_traits.h"

namespace daw {
	template<typename...>
	struct function_info;

	template<typename T, std::enable_if_t<daw::is_callable_v<T>, std::nullptr_t> = nullptr>
	struct function_info : public function_info<decltype( &T::operator( ) )> {};
	// For generic types, directly use the result of the signature of its
	// 'operator()'

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) const> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};
} // namespace daw

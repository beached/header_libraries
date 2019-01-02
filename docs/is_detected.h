
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

#include <type_traits>

namespace daw {
	template<typename...>
	struct voider {
		using type = void;
	};

	template<typename... Ts>
	using void_t = typename voider<Ts...>::type;

	struct nonesuch {
		nonesuch( ) = delete;
		~nonesuch( ) = delete;
		nonesuch( nonesuch const & ) = delete;
		void operator=( nonesuch const & ) = delete;
	};

	namespace impl {
		template<class Default, class AlwaysVoid, template<class...> class Op,
		         class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template<class Default, template<class...> class Op, class... Args>
		struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};
	} // namespace impl

	template<template<class...> class Op, class... Args>
	using is_detected =
	  typename impl::detector<nonesuch, void, Op, Args...>::value_t;

	template<template<class...> class Op, class... Args>
	using detected_t = typename impl::detector<nonesuch, void, Op, Args...>::type;

	template<class Default, template<class...> class Op, class... Args>
	using detected_or = impl::detector<Default, void, Op, Args...>;

	template<template<class...> class Op, class... Args>
	inline constexpr bool is_detected_v = is_detected<Op, Args...>::value;

	template<class Default, template<class...> class Op, class... Args>
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;

	template<class Expected, template<class...> class Op, class... Args>
	using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

	template<class Expected, template<class...> class Op, class... Args>
	inline constexpr bool is_detected_exact_v =
	  is_detected_exact<Expected, Op, Args...>::value;

	template<class To, template<class...> class Op, class... Args>
	using is_detected_convertible =
	  std::is_convertible<detected_t<Op, Args...>, To>;

	template<class To, template<class...> class Op, class... Args>
	inline constexpr bool is_detected_convertible_v =
	  is_detected_convertible<To, Op, Args...>::value;
} // namespace daw



/// Example

template<typename... Ts>
using detect_is_tuple = decltype( tuple_test( std::declval<Ts>( )... ) );


template<typename Tuple>
inline constexpr bool is_tuple_v = daw::is_detected_v<detect_is_tuple, Tuple>;


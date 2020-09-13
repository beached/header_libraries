// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once
#include <cstddef>
#include <type_traits>

namespace daw {
	namespace enable_if_details {
		template<class...>
		struct conjunction : std::true_type {};

		template<class B1>
		struct conjunction<B1> : B1 {};

		template<class B1, class... Bn>
		struct conjunction<B1, Bn...>
		  : std::conditional<bool( B1::value ), conjunction<Bn...>, B1>::type {};

		template<bool>
		struct enable_if {};

		template<>
		struct enable_if<true> {
			using type = std::nullptr_t;
		};
	} // namespace enable_if_details

	template<bool... B>
	using enable_when_t =
	  typename enable_if_details::enable_if<( B and ... )>::type;

	template<typename... Traits>
	using enable_when_all_t = typename enable_if_details::enable_if<
	  enable_if_details::conjunction<Traits...>::value>::type;
} // namespace daw

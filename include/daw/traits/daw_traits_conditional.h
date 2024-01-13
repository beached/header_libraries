// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	namespace traits_details {
		template<bool>
		struct select;

		template<>
		struct select<true> {
			template<typename T, typename>
			using type = T;
		};

		template<>
		struct select<false> {
			template<typename, typename F>
			using type = F;
		};
	} // namespace traits_details

	template<bool B, typename TrueType, typename FalseType>
	using conditional_t = typename traits_details::select<B>::template type<TrueType, FalseType>;
}


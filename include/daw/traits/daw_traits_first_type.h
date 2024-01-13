// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw::traits {
	namespace traits_details {
		template<typename T, typename...>
		struct first_type_impl {
			using type = T;
		};
	} // namespace traits_details

	template<typename... Args>
	using first_type = typename traits_details::first_type_impl<Args...>::type;

} // namespace daw::traits

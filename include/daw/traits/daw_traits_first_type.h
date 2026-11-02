// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

namespace daw::traits {
#if defined( DAW_CPP26_PACK_INDEXING )
	_Pragma( "clang diagnostic push" );
	_Pragma( "clang diagnostic ignored \"-Wc++26-extensions\"" );
	template<typename... Ts>
	using first_type = Ts...[0];
	_Pragma( "clang diagnostic pop" );
#else
	namespace traits_details {
		template<typename T, typename...>
		struct first_type_impl {
			using type = T;
		};
	} // namespace traits_details

	template<typename... Ts>
	using first_type = typename traits_details::first_type_impl<Ts...>::type;

#endif
} // namespace daw::traits
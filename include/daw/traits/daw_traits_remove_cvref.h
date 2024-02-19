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
#if DAW_HAS_BUILTIN( __remove_cv )
	template<typename T>
	struct remove_cv {
		using type = __remove_cv( T );
	};
#else
	template<typename T>
	struct remove_cv {
		using type = T;
	};

	template<typename T>
	struct remove_cv<T const> {
		using type = T;
	};

	template<typename T>
	struct remove_cv<T volatile> {
		using type = T;
	};

	template<typename T>
	struct remove_cv<T const volatile> {
		using type = T;
	};
#endif

	template<typename T>
	using remove_cv_t = typename remove_cv<T>::type;

#if DAW_HAS_BUILTIN( __remove_reference )
	template<typename T>
	struct remove_reference {
		using type = __remove_reference( T );
	};
#else
	template<typename T>
	struct remove_reference {
		using type = T;
	};

	template<typename T>
	struct remove_reference<T &> {
		using type = T;
	};

	template<typename T>
	struct remove_reference<T &&> {
		using type = T;
	};
#endif

	template<typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<typename T>
	using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
} // namespace daw::traits

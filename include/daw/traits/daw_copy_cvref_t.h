// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <type_traits>

namespace daw {

	template<typename From, typename To>
	using copy_reference_t = std::conditional_t<
	  std::is_lvalue_reference_v<From>,
	  std::add_lvalue_reference_t<std::remove_reference_t<To>>,
	  std::conditional_t<std::is_rvalue_reference_v<From>,
	                     std::add_rvalue_reference_t<std::remove_reference_t<To>>,
	                     std::remove_reference_t<To>>>;

	template<typename From, typename To>
	using copy_volatile_t = std::conditional_t<
	  std::is_volatile_v<std::remove_reference_t<From>>,
	  copy_reference_t<To, std::remove_reference_t<To> volatile>, To>;

	template<typename From, typename To>
	using copy_const_t =
	  std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,
	                     copy_reference_t<To, std::remove_reference_t<To> const>,
	                     To>;

	template<typename From, typename To>
	using copy_cv_t = std::conditional_t<
	  std::is_const_v<std::remove_reference_t<From>>,
	  std::conditional_t<
	    std::is_volatile_v<std::remove_reference_t<From>>,
	    copy_reference_t<To, std::remove_reference_t<To> const volatile>,
	    copy_reference_t<To, std::remove_reference_t<To> const>>,
	  std::conditional_t<
	    std::is_volatile_v<std::remove_reference_t<From>>,
	    copy_reference_t<To, std::remove_reference_t<To> volatile>,
	    copy_reference_t<To, std::remove_reference_t<To>>>>;

	template<typename From, typename To>
	using copy_cvref_t = copy_reference_t<From, copy_cv_t<From, To>>;
} // namespace daw

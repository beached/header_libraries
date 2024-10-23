// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "traits/daw_traits_is_same.h"

#include <daw/stdinc/integral_constant.h>
#include <daw/stdinc/is_convertible.h>
#include <daw/stdinc/void_t.h>

namespace daw {

	template<template<typename...> typename, typename...>
	struct nonesuch {
		nonesuch( ) = delete;
		~nonesuch( ) = delete;
		nonesuch( nonesuch && ) = delete;
		nonesuch( nonesuch const & ) = delete;
		nonesuch &operator=( nonesuch && ) = delete;
		nonesuch operator=( nonesuch const & ) = delete;
	};

	template<typename>
	inline constexpr bool is_nonesuch_v = false;

	template<template<class...> class Op, class... Args>
	inline constexpr bool is_nonesuch_v<nonesuch<Op, Args...>> = true;

#if defined( _MSC_VER )
#if _MSC_VER < 1930 and not defined( DAW_NO_CONCEPTS )
#define DAW_NO_CONCEPTS
#endif
#endif

#if defined( __cpp_concepts ) and not defined( DAW_NO_CONCEPTS )
#if __cpp_concepts >= 201907L
#define DAW_HAS_CONCEPTS
#endif
#endif

#ifdef DAW_HAS_CONCEPTS
	template<template<class...> class Op, class... Args>
	inline constexpr bool is_detected_v = requires {
		typename Op<Args...>;
	};
#endif
	namespace is_detect_details {
		template<class Default, class AlwaysVoid, template<class...> class Op,
		         class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template<typename /*Default*/, typename /*AlwaysVoid*/,
		         template<typename...> typename /*Op*/, typename... /*Args*/>
		inline constexpr bool detector_v = false;

		template<typename Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

		template<typename Default, template<typename...> typename Op,
		         typename... Args>
		inline constexpr bool
		  detector_v<Default, std::void_t<Op<Args...>>, Op, Args...> = true;
	} // namespace is_detect_details

	template<template<typename...> typename Op, typename... Args>
	using is_detected =
	  typename is_detect_details::detector<nonesuch<Op, Args...>, void, Op,
	                                       Args...>::value_t;

	template<template<typename...> typename Op, typename... Args>
	using detected_t =
	  typename is_detect_details::detector<nonesuch<Op, Args...>, void, Op,
	                                       Args...>::type;

#ifndef DAW_HAS_CONCEPTS
	template<template<typename...> typename Op, typename... Args>
	inline constexpr bool is_detected_v =
	  is_detect_details::detector_v<nonesuch<Op, Args...>, void, Op, Args...>;

#endif
	template<typename Default, template<typename...> typename Op,
	         typename... Args>
	using detected_or = is_detect_details::detector<Default, void, Op, Args...>;

	template<typename Default, template<typename...> typename Op,
	         typename... Args>
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;

	template<typename Expected, template<typename...> typename Op,
	         typename... Args>
	inline constexpr bool is_detected_exact_v =
	  daw::is_same_v<Expected, detected_t<Op, Args...>>;

	template<typename Expected, template<typename...> typename Op,
	         typename... Args>
	using is_detected_exact =
	  std::bool_constant<is_detected_exact_v<Expected, Op, Args...>>;

	template<typename To, template<typename...> typename Op, typename... Args>
	using is_detected_convertible =
	  std::is_convertible<detected_t<Op, Args...>, To>;

	template<typename To, template<typename...> typename Op, typename... Args>
	inline constexpr bool is_detected_convertible_v =
	  is_detected_convertible<To, Op, Args...>::value;
} // namespace daw

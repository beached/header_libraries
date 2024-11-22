// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#if defined( __cpp_lib_concepts )
#define DAW_CPP20_REQUIRES( ... ) requires( __VA_ARGS__ )
#else
#define DAW_CPP20_REQUIRES( ... )
#endif
#if defined( __cpp_lib_concepts )

#define DAW_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	template<typename T>                                   \
	inline constexpr bool Name = requires {                \
		__VA_ARGS__;                                         \
	}

#define DAW_MAKE_REQ_TRAIT2( Name, /*ReqExpression*/... ) \
	template<typename T, typename U>                        \
	inline constexpr bool Name = requires {                 \
		__VA_ARGS__;                                          \
	}

#define DAW_MAKE_REQ_TRAIT3( Name, /*ReqExpression*/... ) \
	template<typename T, typename U, typename V>            \
	inline constexpr bool Name = requires {                 \
		__VA_ARGS__;                                          \
	}

#define DAW_MAKE_REQ_TRAIT_TYPE( Name, /*ReqExpression*/... ) \
	template<typename T>                                        \
	inline constexpr bool Name = requires {                     \
		typename __VA_ARGS__;                                     \
	}

#define DAW_MAKE_REQ_TRAIT2D( Name, /*ReqExpression*/... ) \
	template<typename T, typename U = T>                     \
	inline constexpr bool Name = requires {                  \
		__VA_ARGS__;                                           \
	}
#else
#define DAW_MAKE_REQ_TRAIT( Name, /*ReqExpression*/... ) \
	template<typename, typename = void>                    \
	inline constexpr bool Name = false;                    \
                                                         \
	template<typename T>                                   \
	inline constexpr bool Name<T, std::void_t<decltype( __VA_ARGS__ )>> = true

#define DAW_MAKE_REQ_TRAIT2( Name, /*ReqExpression*/... )                  \
	template<typename, typename, typename = void>                            \
	inline constexpr bool Name = false;                                      \
                                                                           \
	template<typename T, typename U>                                         \
	inline constexpr bool Name<T, U, std::void_t<decltype( __VA_ARGS__ )>> = \
	  true

#define DAW_MAKE_REQ_TRAIT3( Name, /*ReqExpression*/... )                     \
	template<typename, typename, typename, typename = void>                     \
	inline constexpr bool Name = false;                                         \
                                                                              \
	template<typename T, typename U, typename V>                                \
	inline constexpr bool Name<T, U, V, std::void_t<decltype( __VA_ARGS__ )>> = \
	  true

#define DAW_MAKE_REQ_TRAIT_TYPE( Name, /*ReqExpression*/... ) \
	template<typename, typename = void>                         \
	inline constexpr bool Name = false;                         \
                                                              \
	template<typename T>                                        \
	inline constexpr bool Name<T, std::void_t<typename __VA_ARGS__>> = true

#define DAW_MAKE_REQ_TRAIT2D( Name, /*ReqExpression*/... )                 \
	template<typename T, typename = T, typename = void>                      \
	inline constexpr bool Name = false;                                      \
                                                                           \
	template<typename T, typename U>                                         \
	inline constexpr bool Name<T, U, std::void_t<decltype( __VA_ARGS__ )>> = \
	  true
#endif

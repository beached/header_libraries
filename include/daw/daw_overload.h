// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <ciso646>
#include <type_traits>

namespace daw {
	// overload_t/overload create a callable with the overloads of operator( )
	// provided
	//
	namespace overload_details {
		template<typename>
		struct fp_wrapper_t;

		template<typename ReturnType, typename... Arguments>
		struct fp_wrapper_t<ReturnType ( * )( Arguments... )> {
			std::add_pointer_t<ReturnType( Arguments... )> fp;

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&...args ) const {
				return fp( DAW_FWD2( Args, args )... );
			}
		};

		template<typename ReturnType, typename... Arguments>
		struct fp_wrapper_t<ReturnType ( & )( Arguments... )> {
			std::add_pointer_t<ReturnType( Arguments... )> fp;

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&...args ) const {
				return fp( DAW_FWD2( Args, args )... );
			}
		};

		template<typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template<typename Func>
		using fp_wrapper = std::conditional_t<std::is_class_v<remove_cvref_t<Func>>,
		                                      Func, fp_wrapper_t<Func>>;

		template<auto>
		struct lift_t;

		template<typename ReturnType, typename... Arguments,
		         ReturnType ( *fp )( Arguments... )>
		struct lift_t<fp> {
			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&...args ) const
			  noexcept( noexcept( fp( DAW_FWD2( Args, args )... ) ) ) {
				return fp( DAW_FWD2( Args, args )... );
			}
		};

	} // namespace overload_details

	template<auto fp>
	inline constexpr overload_details::lift_t<fp> lift =
	  overload_details::lift_t<fp>{ };

	template<typename... Funcs>
	struct overload : overload_details::fp_wrapper<Funcs>... {
		using overload_details::fp_wrapper<Funcs>::operator( )...;

		constexpr overload( Funcs... fs )
		  : overload_details::fp_wrapper<Funcs>{ fs }... {}
	};

	template<typename... Funcs>
	overload( Funcs &&... )
	  -> overload<std::conditional_t<std::is_class_v<remove_cvref_t<Funcs>>,
	                                 remove_cvref_t<Funcs>, Funcs>...>;
} // namespace daw

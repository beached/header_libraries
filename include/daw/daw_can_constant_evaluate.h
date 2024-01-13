// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries/
//

#pragma once

#include "daw_cpp_feature_check.h"
#include "daw_traits.h"
#include "daw_typeof.h"
#include "traits/daw_traits_conditional.h"

#include <type_traits>

#if defined( __cpp_nontype_template_args )
#if __cpp_nontype_template_args > 201911L
#define DAW_CXEVAL_FTYPE auto
#else
// Can only pass references/pointers to class types prior to C++20
#define DAW_CXEVAL_FTYPE auto const &
#endif
#endif

namespace daw {
	namespace can_cxeval_impl {
		template<bool, typename T = int>
		using always_t = int;

		template<DAW_CXEVAL_FTYPE F>
		constexpr std::true_type cxeval_test( always_t<( F( ), true )> ) {
			return { };
		}

		template<DAW_CXEVAL_FTYPE>
		constexpr std::false_type cxeval_test( ... ) {
			return { };
		}

		template<DAW_CXEVAL_FTYPE F>
		constexpr auto can_cxeval( ) {
			static_assert(
			  std::is_invocable_v<std::remove_reference_t<decltype( F )>>,
			  "Test function must be invocable without arguments" );
			return cxeval_test<F>( always_t<true>{ } );
		}

		template<typename T, typename... Args>
		struct construct {
			constexpr T operator( )( ) const {
				if constexpr( std::is_aggregate_v<T> ) {
					return T{ Args{ }... };
				} else {
					return T( Args{ }... );
				}
			}
		};
		template<typename T, typename... Args>
		inline constexpr auto construct_v = construct<T, Args...>{ };

		template<typename T, typename... Args>
		struct can_potentially_constant_construct_impl {
			using type = DAW_TYPEOF( can_cxeval<construct_v<T, Args...>>( ) );
		};
	} // namespace can_cxeval_impl

	template<DAW_CXEVAL_FTYPE F>
	using can_constant_evaluate =
	  typename decltype( can_cxeval_impl::can_cxeval<F>( ) )::type;

	template<DAW_CXEVAL_FTYPE F>
	inline constexpr bool can_constant_evaluate_v =
	  can_constant_evaluate<F>::value;

	/***
	 * Checks if T is constexpr constructible from Args.
	 * @pre Args must all be default constructible, if not false is returned
	 */
	template<typename T, typename... Args>
	using can_potentially_constant_construct = typename conditional_t<
	  ( std::is_default_constructible_v<Args> and ... ),
	  can_cxeval_impl::can_potentially_constant_construct_impl<T, Args...>,
	  daw::traits::identity<std::false_type>>::type;

	/***
	 * Checks if T is constexpr constructible from Args.
	 * @pre Args must all be default constructible, if not false is returned
	 */
	template<typename T, typename... Args>
	inline constexpr bool can_potentially_constant_construct_v =
	  can_potentially_constant_construct<T, Args...>::value;
} // namespace daw

#undef DAW_CXEVAL_FTYPE

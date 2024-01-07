// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_bit_cast.h"
#include "daw_cpp_feature_check.h"
#include "daw_deduced_type.h"
#include "daw_enable_if.h"
#include "daw_is_detected.h"
#include "daw_move.h"
#include "impl/daw_conditional.h"

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template<bool B>
	using bool_constant = std::bool_constant<B>;

	template<typename B>
	using negation = std::bool_constant<not bool( B::value )>;

	template<typename...>
	struct conjunction : std::true_type {};

	template<typename B1>
	struct conjunction<B1> : B1 {};

	template<typename B1, typename... Bn>
	struct conjunction<B1, Bn...>
	  : conditional_t<static_cast<bool>( B1::value ), conjunction<Bn...>, B1> {};

	template<typename... T>
	using conjunction_t = typename conjunction<T...>::type;

	template<typename... T>
	inline constexpr bool const conjunction_v = conjunction<T...>::value;

	template<bool... values>
	inline constexpr bool all_true_v = ( values and ... );

	template<bool... values>
	using all_true = std::bool_constant<( values and ... )>;

	template<bool... values>
	inline constexpr bool any_true_v = ( values or ... );

	template<bool... values>
	using any_true = std::bool_constant<( values or ... )>;

	namespace cpp_17_details {
		template<typename Function>
		class not_fn_t {
			Function m_function;

		public:
			not_fn_t( ) = default;

			explicit constexpr not_fn_t( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : m_function{ DAW_MOVE( func ) } {}

			explicit constexpr not_fn_t( Function const &func ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : m_function{ func } {}

			template<typename... Args>
			[[nodiscard]] constexpr decltype( auto )
			operator( )( Args &&...args ) noexcept(
			  std::is_nothrow_invocable_v<Function, Args...> ) {
				return not m_function( DAW_FWD( args )... );
			}

			template<typename... Args>
			[[nodiscard]] constexpr decltype( auto )
			operator( )( Args &&...args ) const
			  noexcept( std::is_nothrow_invocable_v<Function, Args...> ) {
				return not m_function( DAW_FWD( args )... );
			}
		};
	} // namespace cpp_17_details

	template<typename Function>
	[[nodiscard]] constexpr auto not_fn( Function &&func ) {
		using func_t = remove_cvref_t<Function>;
		return cpp_17_details::not_fn_t<func_t>( DAW_FWD( func ) );
	}

	template<typename Function>
	[[nodiscard]] constexpr auto not_fn( ) {
		return cpp_17_details::not_fn_t<Function>( );
	}

	template<typename>
	inline constexpr bool is_reference_wrapper_v = false;

	template<typename T>
	inline constexpr bool is_reference_wrapper_v<std::reference_wrapper<T>> =
	  true;

	template<typename T>
	using is_reference_wrapper = std::bool_constant<is_reference_wrapper_v<T>>;

	template<typename T>
	using not_trait = std::integral_constant<bool, not T::value>;

	template<typename T>
	[[nodiscard]] constexpr std::add_const_t<T> &as_const( T &t ) noexcept {
		return t;
	}

	template<typename Container>
	[[nodiscard]] constexpr auto
	size( Container const &c ) noexcept( noexcept( c.size( ) ) )
	  -> decltype( c.size( ) ) {
		return c.size( );
	}

#ifdef __cpp_lib_is_swappable
	template<typename T>
	inline constexpr bool is_swappable_v = std::is_swappable_v<T>;
#else
	namespace detectors {
		template<typename T>
		using detect_std_swap =
		  decltype( std::swap( std::declval<T &>( ), std::declval<T &>( ) ) );

		template<typename T>
		using detect_adl_swap =
		  decltype( swap( std::declval<T &>( ), std::declval<T &>( ) ) );
	} // namespace detectors

	template<typename T>
	inline constexpr bool is_swappable_v =
	  is_detected_v<detectors::detect_std_swap, T> or
	  is_detected_v<detectors::detect_adl_swap, T>;
#endif

	// Iterator movement, until I can use c++ 17 and the std ones are constexpr
	namespace cpp_17_details {
		// Pointer calc helpers.  Cannot include math header as it depends on
		// algorithm
		namespace math {
#ifdef min
			// MSVC seems to define this :(
#undef min
#endif
			template<typename T, typename U>
			[[nodiscard]] constexpr std::common_type_t<T, U>( min )(
			  T const &lhs, U const &rhs ) noexcept {
				if( lhs <= rhs ) {
					return lhs;
				}
				return rhs;
			}

			template<typename T, typename U, typename V>
			[[nodiscard]] constexpr T clamp( T val, U const &min_val,
			                                 V const &max_val ) noexcept {
				if( val < min_val ) {
					val = min_val;
				} else if( val > max_val ) {
					val = max_val;
				}
				return val;
			}
		} // namespace math
	} // namespace cpp_17_details

	template<typename...>
	struct disjunction : std::false_type {};

	template<typename B1>
	struct disjunction<B1> : B1 {};

	template<typename B1, typename... Bn>
	struct disjunction<B1, Bn...>
	  : conditional_t<bool( B1::value ), B1, disjunction<Bn...>> {};

	template<typename... B>
	using disjunction_t = typename disjunction<B...>::type;

	template<typename... B>
	inline constexpr bool disjunction_v = disjunction<B...>::value;

	namespace cpp_17_details {
		template<typename From, typename To,
		         bool = disjunction_v<std::is_void<From>, std::is_function<To>,
		                              std::is_array<To>>>
		struct do_is_nothrow_convertible {
			using type = std::is_void<To>;
		};

		struct do_is_nothrow_convertible_impl {
			template<typename To>
			static void test_aux( To ) noexcept;

			template<typename From, typename To>
			static bool_constant<noexcept( test_aux<To>( std::declval<From>( ) ) )>
			test( int );

			template<typename, typename>
			static std::false_type test( ... );
		};

		template<typename From, typename To>
		struct do_is_nothrow_convertible<From, To, false> {
			using type =
			  decltype( do_is_nothrow_convertible_impl::test<From, To>( 0 ) );
		};
	} // namespace cpp_17_details

	template<typename From, typename To>
	struct is_nothrow_convertible
	  : cpp_17_details::do_is_nothrow_convertible<From, To>::type {};

	template<typename From, typename To>
	inline constexpr bool is_nothrow_convertible_v =
	  is_nothrow_convertible<From, To>::value;

	template<class T>
	[[nodiscard]] constexpr auto
	decay_copy( T &&v ) noexcept( is_nothrow_convertible_v<T, std::decay_t<T>> )
	  -> std::enable_if_t<std::is_convertible_v<T, std::decay_t<T>>,
	                      std::decay_t<T>> {

		return DAW_FWD( v );
	}
} // namespace daw
#ifdef DAW_HAS_CONCEPTS
#undef DAW_HAS_CONCEPTS
#endif

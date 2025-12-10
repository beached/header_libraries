// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_callable.h"
#include "daw/daw_empty.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/traits/daw_traits_identity.h"
#include "daw/traits/daw_traits_nth_element.h"
#include "daw/traits/daw_traits_pack_list.h"
#include "daw/impl/daw_make_trait.h"

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/declval.h>
#include <daw/stdinc/iterator_traits.h>
#include <daw/stdinc/range_access.h>
#include <type_traits>

namespace daw {
	namespace traits {
		template<typename Function, typename... Args>
		inline constexpr bool is_callable_v = daw::is_callable_v<Function, Args...>;

		// the single void test is used in places like expected.   Comes up with
		// is_callable_v<DAW_TYPEOF( foo( args... ) )>
		template<typename Function>
		inline constexpr bool is_callable_v<Function, void> =
		  is_callable_v<Function>;

		template<typename Result, typename Function, typename... Args>
		inline constexpr bool is_callable_convertible_v =
		  daw::is_callable_r_v<Result, Function, Args...>;

		template<typename Result, typename Function, typename... Args>
		inline constexpr bool
		  is_callable_convertible_v<Result, Function( Args... )> =
		    is_callable_convertible_v<Result, Function, Args...>;

		template<typename UnaryPredicate, typename T>
		inline constexpr bool is_unary_prdicate_v =
		  daw::is_callable_r_v<bool, UnaryPredicate, T>;

		template<typename BinaryPredicate, typename T, typename U>
		inline constexpr bool is_binary_prdicate_v =
		  daw::is_callable_r_v<bool, BinaryPredicate, T, U>;

		template<typename Function, typename... Args>
		inline constexpr bool is_nothrow_callable_v =
		  daw::is_nothrow_callable_v<Function, Args...>;

		template<typename T>
		using make_fp = std::add_pointer_t<T>;

		DAW_MAKE_REQ_TRAIT2D( has_addition_operator_v,
		                      std::declval<T>( ) + std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_subtraction_operator_v,
		                      std::declval<T>( ) - std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_multiplication_operator_v,
		                      std::declval<T>( ) * std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_division_operator_v,
		                      std::declval<T>( ) / std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_compound_assignment_add_operator_v,
		                      std::declval<T &>( ) += std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_compound_assignment_sub_operator_v,
		                      std::declval<T &>( ) -= std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_compound_assignment_mul_operator_v,
		                      std::declval<T &>( ) *= std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_compound_assignment_div_operator_v,
		                      std::declval<T &>( ) /= std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( has_modulus_operator_v,
		                      std::declval<T>( ) % std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT( has_increment_operator_v, ++std::declval<T &>( ) );

		DAW_MAKE_REQ_TRAIT( has_decrement_operator_v, --std::declval<T &>( ) );

		DAW_MAKE_REQ_TRAIT( has_integer_subscript_v, std::declval<T>( )[0U] );

		DAW_MAKE_REQ_TRAIT(
		  has_size_memberfn_v,
		  std::declval<size_t &>( ) = std::declval<T>( ).size( ) );

		DAW_MAKE_REQ_TRAIT(
		  has_empty_memberfn_v,
		  std::declval<bool &>( ) = std::declval<T const &>( ).empty( ) );

		DAW_MAKE_REQ_TRAIT2D( has_append_memberfn_v,
		                      std::declval<T>( ).append( std::declval<U>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_append_operator_v,
		                    std::declval<T &>( ) += std::declval<T>( )[0U] );

		DAW_MAKE_REQ_TRAIT2D( is_equality_comparable_v,
		                      std::declval<bool &>( ) = std::declval<T>( ) ==
		                                                std::declval<U>( ) );
		template<typename T, typename U = T>
		using is_equality_comparable =
		  std::bool_constant<is_equality_comparable_v<T, U>>;

		DAW_MAKE_REQ_TRAIT2D( is_inequality_comparable_v,
		                      std::declval<T>( ) != std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( is_less_than_comparable_v,
		                      std::declval<T>( ) < std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( is_equal_less_than_comparable_v,
		                      std::declval<T>( ) <= std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( is_greater_than_comparable_v,
		                      std::declval<T>( ) > std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2D( is_equal_greater_than_comparable_v,
		                      std::declval<T>( ) >= std::declval<U>( ) );

		template<typename Iterator,
		         typename T = typename std::iterator_traits<Iterator>::value_type,
		         typename = void>
		inline constexpr bool is_assignable_iterator_v = false;

		template<typename Iterator, typename T>
		inline constexpr bool is_assignable_iterator_v<
		  Iterator,
		  T,
		  std::void_t<decltype( *std::declval<Iterator>( ) =
		                          std::declval<T>( ) )>> = true;

		template<typename L, typename R>
		inline constexpr bool is_comparable_v =
		  is_equality_comparable_v<L, R> and is_equality_comparable_v<R, L>;

		DAW_MAKE_REQ_TRAIT( has_std_begin_v, std::begin( std::declval<T>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_adl_begin_v, begin( std::declval<T>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_std_end_v, std::end( std::declval<T>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_adl_end_v, end( std::declval<T>( ) ) );

		template<typename Container>
		inline constexpr bool has_begin =
		  has_std_begin_v<Container> or has_adl_begin_v<Container>;

		template<typename Container>
		inline constexpr bool has_end =
		  has_std_end_v<Container> or has_adl_end_v<Container>;

		template<typename Container>
		inline constexpr bool is_container_like_v =
		  has_begin<Container> and has_end<Container>;

		namespace traits_details {
			namespace is_iter {
				template<typename T>
				using has_value_type = typename std::iterator_traits<T>::value_type;

				template<typename T>
				using has_difference_type =
				  typename std::iterator_traits<T>::difference_type;

				template<typename T>
				using has_reference = typename std::iterator_traits<T>::reference;

				template<typename T>
				using has_pointer = typename std::iterator_traits<T>::pointer;

				template<typename T>
				using has_iterator_category =
				  typename std::iterator_traits<T>::iterator_category;
			} // namespace is_iter

			DAW_MAKE_REQ_TRAIT( is_incrementable_v,
			                    std::declval<T &>( ) = ++std::declval<T &>( ) );

			DAW_MAKE_REQ_TRAIT_TYPE( has_value_type_v, is_iter::has_value_type<T> );

			DAW_MAKE_REQ_TRAIT_TYPE( has_difference_type_v,
			                         is_iter::has_difference_type<T> );

			DAW_MAKE_REQ_TRAIT_TYPE( has_reference_v, is_iter::has_reference<T> );

			DAW_MAKE_REQ_TRAIT_TYPE( has_pointer_v, is_iter::has_pointer<T> );

			DAW_MAKE_REQ_TRAIT_TYPE( has_iterator_category_v,
			                         is_iter::has_iterator_category<T> );

			template<typename T>
			inline constexpr bool has_iterator_trait_types_v =
			  has_value_type_v<T> and has_difference_type_v<T> and
			  has_reference_v<T> and has_pointer_v<T> and has_iterator_category_v<T>;
		} // namespace traits_details

		DAW_MAKE_REQ_TRAIT( is_dereferenceable_v, *std::declval<T>( ) );

		template<typename T>
		using is_dereferenceable = std::bool_constant<is_dereferenceable_v<T>>;
	} // namespace traits

	namespace traits_details {
		template<typename, typename = std::nullptr_t>
		struct void_function;

		template<typename Function>
		struct void_function<
		  Function,
		  std::enable_if_t<std::is_default_constructible_v<Function>,
		                   std::nullptr_t>> {

			Function function;

			constexpr void_function( ) noexcept(
			  std::is_nothrow_constructible_v<Function> ) = default;

			explicit constexpr void_function( Function const &func ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : function( func ) {}

			explicit constexpr void_function( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : function( std::move( func ) ) {}

			explicit constexpr operator bool( ) noexcept(
			  noexcept( static_cast<bool>( std::declval<Function>( ) ) ) ) {

				return static_cast<bool>( function );
			}

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&...args ) noexcept(
			  traits::is_nothrow_callable_v<Function, Args...> ) {

				function( DAW_FWD( args )... );
			}
		};

		template<typename Function>
		struct void_function<
		  Function,
		  std::enable_if_t<not std::is_default_constructible_v<Function>,
		                   std::nullptr_t>> {

			Function function;

			explicit constexpr void_function( Function const &func ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : function( func ) {}

			explicit constexpr void_function( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : function( std::move( func ) ) {}

			explicit constexpr operator bool( ) noexcept(
			  noexcept( static_cast<bool>( std::declval<Function>( ) ) ) ) {

				return static_cast<bool>( function );
			}

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&...args ) noexcept(
			  traits::is_nothrow_callable_v<Function, Args...> ) {
				function( DAW_FWD( args )... );
			}
		};

	} // namespace traits_details

	template<typename Function>
	[[nodiscard]] constexpr auto make_void_function( Function &&func ) noexcept(
	  noexcept( traits_details::void_function<Function>( DAW_FWD( func ) ) ) ) {

		return traits_details::void_function<Function>( DAW_FWD( func ) );
	}

	template<size_t N, typename... Args>
	using type_n_t = traits::nth_element<N, Args...>;

	template<bool Condition, typename IfTrue, typename IfFalse>
	struct type_if_else {
		using type = IfFalse;
	};

	template<typename IfTrue, typename IfFalse>
	struct type_if_else<true, IfTrue, IfFalse> {
		using type = IfTrue;
	};

	template<bool Condition, typename IfTrue, typename IfFalse>
	using if_else_t = typename type_if_else<Condition, IfTrue, IfFalse>::type;

	template<bool B, typename T = std::nullptr_t>
	using required = std::enable_if_t<B, T>;

	template<size_t N, typename... Args>
	struct pack_type {
		using type = std::decay_t<traits::nth_element<N, Args...>>;
	};

	template<size_t N, typename... Args>
	using pack_type_t = typename pack_type<N, Args...>::type;
	namespace traits_details {
		struct non_constructor {};

		template<bool>
		struct delete_default_constructor_if {
			delete_default_constructor_if( ) = default;

			explicit constexpr delete_default_constructor_if(
			  non_constructor ) noexcept {}
		};

		template<>
		struct delete_default_constructor_if<true> {
			explicit constexpr delete_default_constructor_if(
			  non_constructor ) noexcept {}
		};

		template<bool>
		struct delete_copy_constructor_if {
			delete_copy_constructor_if( ) = default;
			constexpr delete_copy_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_copy_constructor_if<true> {

			explicit constexpr delete_copy_constructor_if(
			  non_constructor ) noexcept {}

			delete_copy_constructor_if( ) = default;
			~delete_copy_constructor_if( ) = default;
			delete_copy_constructor_if( delete_copy_constructor_if const & ) = delete;
			delete_copy_constructor_if( delete_copy_constructor_if && ) = default;

			delete_copy_constructor_if &
			operator=( delete_copy_constructor_if const & ) = default;

			delete_copy_constructor_if &
			operator=( delete_copy_constructor_if && ) = default;
		};

		template<bool>
		struct delete_copy_assignment_if {
			delete_copy_assignment_if( ) = default;
			explicit constexpr delete_copy_assignment_if( non_constructor ) noexcept {
			}
		};

		template<>
		struct delete_copy_assignment_if<true> {
			explicit constexpr delete_copy_assignment_if( non_constructor ) noexcept {
			}

			delete_copy_assignment_if( ) = default;
			~delete_copy_assignment_if( ) = default;
			delete_copy_assignment_if( delete_copy_assignment_if const & ) = default;
			delete_copy_assignment_if( delete_copy_assignment_if && ) = default;

			delete_copy_assignment_if &
			operator=( delete_copy_assignment_if const & ) = delete;

			delete_copy_assignment_if &
			operator=( delete_copy_assignment_if && ) = default;
		};

		template<bool>
		struct delete_move_constructor_if {
			delete_move_constructor_if( ) = default;
			explicit constexpr delete_move_constructor_if(
			  non_constructor ) noexcept {}
		};

		template<>
		struct delete_move_constructor_if<true> {
			explicit constexpr delete_move_constructor_if(
			  non_constructor ) noexcept {}

			delete_move_constructor_if( ) = default;
			~delete_move_constructor_if( ) = default;
			delete_move_constructor_if( delete_move_constructor_if && ) = delete;
			delete_move_constructor_if( delete_move_constructor_if const & ) =
			  default;

			delete_move_constructor_if &
			operator=( delete_move_constructor_if const & ) = default;

			delete_move_constructor_if &
			operator=( delete_move_constructor_if && ) = default;
		};

		template<bool>
		struct delete_move_assignment_if {
			delete_move_assignment_if( ) = default;
			explicit constexpr delete_move_assignment_if( non_constructor ) noexcept {
			}
		};

		template<>
		struct delete_move_assignment_if<true> {
			explicit constexpr delete_move_assignment_if( non_constructor ) noexcept {
			}

			delete_move_assignment_if( ) = default;
			~delete_move_assignment_if( ) = default;
			delete_move_assignment_if( delete_move_assignment_if const & ) = default;
			delete_move_assignment_if( delete_move_assignment_if && ) = default;

			delete_move_assignment_if &
			operator=( delete_move_assignment_if const & ) = default;

			delete_move_assignment_if &
			operator=( delete_move_assignment_if && ) = delete;
		};
	} // namespace traits_details

	template<typename T, bool B = true>
	using enable_default_constructor =
	  traits_details::delete_default_constructor_if<
	    not std::is_default_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_copy_constructor = traits_details::delete_copy_constructor_if<
	  not std::is_copy_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_copy_assignment = traits_details::delete_copy_assignment_if<
	  not std::is_copy_assignable_v<T> and B>;

	template<typename T, bool B = true>
	using enable_move_constructor = traits_details::delete_move_constructor_if<
	  not std::is_move_constructible_v<T> and B>;

	template<typename T, bool B = true>
	using enable_move_assignment = traits_details::delete_move_assignment_if<
	  not std::is_move_assignable_v<T> and B>;
} // namespace daw

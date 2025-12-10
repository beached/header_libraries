// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "cpp_17.h"
#include "daw_arith_traits.h"
#include "daw_cpp_feature_check.h"
#include "daw_iterator_traits.h"
#include "daw_move.h"

#include <concepts>
#include <functional>
#include <iterator>
#include <type_traits>

namespace daw {
	/***
	 * @brief Given types From and To and an expression E whose type and value
	 * category are the same as those of std::declval<From>(),
	 * convertible_to<From, To> requires E to be both implicitly and explicitly
	 * convertible to type To. The implicit and explicit conversions are required
	 * to produce equal results.
	 */
	template<typename From, typename To>
	concept convertible_to = std::is_convertible_v<From, To> and requires {
		{ static_cast<To>( std::declval<From>( ) ) };
	};

	/***
	 * @brief Given types From and To and an expression E whose type and value
	 * category are the same as those of std::declval<From>(),
	 * implicitly_convertible_to<From, To> requires E to be implicitly
	 * convertible to type To.
	 */
	template<typename From, typename To>
	concept implicitly_convertible_to = std::is_convertible_v<From, To>;

	/***
	 * @brief Satisfied when Lhs and Rhs name the same type (taking into account
	 * const/volatile qualifications)
	 */
	template<typename Lhs, typename Rhs>
	concept same_as = std::is_same_v<Lhs, Rhs>;

	namespace deprecated {
		template<typename Lhs, typename Rhs>
		[[deprecated( "use cvref_of" )]] inline constexpr bool same_as_rrcv_v =
		  same_as<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>;
	}

	template<typename Lhs, typename Rhs>
	concept same_as_rrcv = deprecated::same_as_rrcv_v<Lhs, Rhs>;

	template<typename Arg, typename Class>
	concept cvref_of =
	  same_as<std::remove_cvref_t<Arg>, std::remove_cvref_t<Class>>;

	namespace deprecated {
		template<typename Arg, typename Class>
		[[deprecated( "Use not_cvref_of" )]] inline constexpr bool not_me_v =
		  not same_as<Class, std::remove_cvref_t<Arg>>;
	}

	template<typename Arg, typename Class>
	concept not_me = deprecated::not_me_v<Arg, Class>;

	template<typename Arg, typename Class>
	concept not_cvref_of = not cvref_of<Arg, Class>;

	namespace deprecated {
		template<typename Arg, typename Class>
		[[deprecated( "Use not_decay_of" )]] inline constexpr bool not_me_d_v =
		  same_as<Class, std::decay_t<Arg>>;
	}
	template<typename Arg, typename Class>
	concept not_me_d = deprecated::not_me_d_v<Arg, Class>;

	template<typename Arg, typename Class>
	concept not_decay_of = not same_as<Class, std::decay_t<Arg>>;

	template<typename T>
	using root_type_t = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;

	/***
	 * @brief The constructible_from concept constrains the initialization of a
	 * variable of a given type with a particular set of argument types.
	 */
	template<typename To, typename... Args>
	concept constructible_from =
	  std::destructible<To> and requires( Args && ...args ) {
		To{ DAW_FWD( args )... };
	}; // use std::is_constructible_v<T, Args...>;
	   // once clang supports it for aggregates

	template<typename T>
	concept Pointers = std::is_pointer_v<T>;

	template<typename T>
	concept Containers = requires( T container ) {
		typename std::remove_cvref_t<T>::value_type;
		container.begin( );
		container.end( );
	};

	template<typename T>
	concept ContiguousContainer = requires( T && container ) {
		{ std::data( container ) } -> Pointers;
		{ std::size( container ) } -> convertible_to<std::size_t>;
	};

	template<typename T, typename U>
	concept ContiguousContainerOf =
	  ContiguousContainer<T> and requires( T container ) {
		{ *std::data( container ) } -> convertible_to<U>;
	};

	template<typename T>
	concept StringLike =
	  Containers<std::remove_cvref_t<T>> and
	  same_as<typename std::remove_cvref_t<T>::value_type, char>;

	template<typename T>
	concept EnumType =
	  std::is_enum_v<std::remove_cvref_t<std::remove_cvref_t<T>>>;

	template<typename T>
	concept UnsignedStd = std::is_unsigned_v<std::remove_cvref_t<T>>;

	template<typename T>
	concept SignedStd = std::is_signed_v<std::remove_cvref_t<T>>;

	template<typename T>
	concept IntegralStd = std::is_integral_v<std::remove_cvref_t<T>>;

	template<typename T>
	concept IntegralDaw = daw::is_integral_v<std::remove_cvref_t<T>>;

	template<typename T>
	concept FloatingPointStd = std::is_floating_point_v<std::remove_cvref_t<T>>;

	template<typename T>
	concept FloatingPointDaw = daw::is_floating_point_v<std::remove_cvref_t<T>>;

	/// @brief Specifies that Container has type alias members key_type and
	/// mapped_type
	template<typename Container>
	inline constexpr bool has_kv_mapping_v = requires {
		typename std::remove_cvref_t<Container>::key_type;
		typename std::remove_cvref_t<Container>::mapped_type;
	};

	template<typename T>
	concept AssociativeContainers = Containers<std::remove_cvref_t<T>> and
	                                has_kv_mapping_v<std::remove_cvref_t<T>>;

	/***
	 * Specifies that a callable type F can be called with a set of argument
	 * Args... using the function template std::invoke.
	 * @tparam Func Callable to test
	 * @tparam Args Arguments to call callable with
	 */
	template<typename Func, typename... Args>
	concept invocable = requires( Func && f, Args &&...args ) {
		std::invoke( DAW_FWD( f ), DAW_FWD( args )... );
	};

	/// @brief Specifies that an expression of the type and value category
	/// specified by RHS can be assigned to an lvalue expression whose type is
	/// specified by LHS.
	template<typename LHS, typename RHS>
	concept assignable_from =
#if defined( __cpp_lib_concepts )
	  std::assignable_from<LHS, RHS>;
#else
	  std::is_lvalue_reference_v<LHS> and requires( LHS lhs, RHS &&rhs ) {
		{ lhs = DAW_FWD( rhs ) } -> std::same_as<LHS>;
	};
#endif

	/// @brief Satisfied if T is a reference type, or if it is an object type
	/// where an object of that type can be constructed from an rvalue of that
	/// type in both direct- and copy-initialization contexts, with the usual
	/// semantics
	template<typename T>
	concept move_constructible =
#if defined( __cpp_lib_concepts )
	  std::move_constructible<T>;
#else
	  constructible_from<T, T> and convertible_to<T, T>;
#endif

#if not defined( __cpp_lib_concepts )
	namespace swappable_test {
		using namespace std;
		template<typename T>
		inline constexpr bool swappable_test = requires( T & a, T &b ) {
			swap( a, b );
		};
	} // namespace swappable_test
#endif

	/// @brief Specifies that expressions of the type and value category
	/// encoded by T and U are swappable with each other.
	template<typename T>
	concept swappable =
#if defined( __cpp_lib_concepts )
	  std::swappable<T>;
#else
	  swappable_test::swappable_test<T>;
#endif

	/// @brief Specifies that T is an object type that can be moved (that is,
	/// it can be move constructed, move assigned, and lvalues of type T can
	/// be swapped).
	template<typename T>
	concept movable =
#if defined( __cpp_lib_concepts )
	  std::movable<T>;
#else
	  std::is_object_v<T> and move_constructible<T> and
	  assignable_from<T &, T> and swappable<T>;
#endif

	template<typename I>
	concept weakly_incrementable =
#if defined( __cpp_lib_concepts )
	  std::weakly_incrementable<I>;
#else
	  movable<I> and requires( I i ) {
		typename iter_difference_t<I>;
		requires SignedStd<iter_difference_t<I>>;
		{ ++i } -> same_as<I &>;
		i++;
	};
#endif

	/***
	 * Specifies that a callable type F can be called with a set of argument
	 * Args... using the function template std::invoke. The Result of the
	 * invocation must be convertible to Result
	 * @tparam Result Excepted result type
	 * @tparam Func Callable to test
	 * @tparam Args Arguments to call callable with
	 */
	template<typename Func, typename Result, typename... Args>
	concept invocable_result = requires( Func && f, Args &&...args ) {
		{
			std::invoke( DAW_FWD( f ), DAW_FWD( args )... )
		} -> convertible_to<Result>;
	};

	template<typename I>
	concept input_or_output_iterator =
#if defined( __cpp_lib_concepts )
	  std::input_or_output_iterator<I>;
#else
	  requires( I i ) {
		{ *i };
	}
	and weakly_incrementable<I>;
#endif

	template<typename Out, typename T>
	concept indirectly_writable =
#if defined( __cpp_lib_concepts )
	  std::indirectly_writable<Out, T>;
#else
	  requires( Out && o, T &&t ) {
		*o = DAW_FWD( t );
		*DAW_FWD( o ) = DAW_FWD( t );
		const_cast<const iter_reference_t<Out> &&>( *o ) = DAW_FWD( t );
		const_cast<const iter_reference_t<Out> &&>( *DAW_FWD( o ) ) = DAW_FWD( t );
	};
#endif

	template<typename Derived, typename Base>
	concept derived_from =
	  std::is_base_of_v<Base, Derived> and
	  std::is_convertible_v<const volatile Derived *, const volatile Base *>;

	template<typename I, typename T>
	concept output_iterator =
#if defined( __cpp_lib_concepts )
	  std::output_iterator<I, T>;
#else
	  input_or_output_iterator<I> and indirectly_writable<I, T> and
	  requires( I i, T &&t ) {
		*i++ = DAW_FWD( t ); // not required to be equality-preserving
	};
#endif

	template<typename T>
	using iterator_category_t =
	  typename std::iterator_traits<T>::iterator_category;

	template<typename T>
	using iter_reference_type = iter_reference_t<T>;

	template<typename T>
	using iter_value_type = iter_value_t<T>;

	template<typename I>
	concept input_iterator =
#if defined( __cpp_lib_concepts )
	  std::input_iterator<I>;
#else
	  input_or_output_iterator<I> and
	  not same_as<iterator_category_t<I>, void> and
	  derived_from<iterator_category_t<I>, std::input_iterator_tag>;
#endif

#if defined( __cpp_lib_concepts )
	template<typename I>
	concept forward_iterator = std::forward_iterator<I>;
#else
	template<typename I>
	concept forward_iterator =
	  iterator<I> and
	  derived_from<iterator_category_t<I>, std::forward_iterator_tag>;
#endif

#if defined( __cpp_lib_concepts )
	template<typename I>
	concept random_access_iterator = std::random_access_iterator<I>;
#else
	template<typename I>
	concept random_access_iterator =
	  forward_iterator<I> and
	  derived_from<iterator_category_t<I>, std::random_access_iterator_tag>;
#endif

	template<typename Container>
	concept BackInsertableContainer =
	  Containers<Container> and
	  requires( Container c, typename Container::value_type const &v ) {
		c.push_back( v );
	};

#if not defined( __cpp_lib_concepts )
	namespace concept_details {
		template<typename B>
		concept boolean_testable_impl = convertible_to<B, bool>;
	}

	/// @brief Specifies the requirements for expressions that are convertible
	/// to bool and for which the logical operators have the usual behavior
	/// (including short-circuiting), even for two different boolean-testable
	/// types
	template<typename B>
	concept boolean_testable =
	  concept_details::boolean_testable_impl<B> and requires( B && b ) {
		{ not DAW_FWD( b ) } -> concept_details::boolean_testable_impl;
	};

	namespace concept_details {
		template<typename T, typename U>
		concept weakly_equality_comparable_with =
		  requires( std::remove_reference_t<T> const &t,
		            std::remove_reference_t<U> const &u ) {
			{ t == u } -> boolean_testable;
			{ t != u } -> boolean_testable;
			{ u == t } -> boolean_testable;
			{ u != t } -> boolean_testable;
		};
	} // namespace concept_details
#endif

	/// @brief Specifies that the comparison operators == and != on T reflects
	/// equality: == yields true if and only if the operands are equal
	template<typename T>
	concept equality_comparable =
#if defined( __cpp_lib_concepts )
	  std::equality_comparable<T>;
#else
	  concept_details::weakly_equality_comparable_with<T, T>;
#endif

	/// @brief Specifies that the comparison operators == and != on (possibly
	/// mixed) T and U operands yield results consistent with equality.
	/// Comparing mixed operands yields results equivalent to comparing the
	/// operands converted to their common type.
	template<class T, class U>
	concept equality_comparable_with =
#if defined( __cpp_lib_concepts )
	  std::equality_comparable_with<T, U>;
#else
	  concept_details::weakly_equality_comparable_with<T, U>;
#endif

	template<typename T>
	concept Allocators = true;

	template<typename Alloc, typename = void>
	inline constexpr bool is_move_insertable_v =
	  std::is_move_constructible_v<typename Alloc::value_type>;

	template<typename T>
	concept default_constructible = requires {
		T{ };
	};

	template<typename T, typename SpecificInt = void>
	concept Integer = (same_as<SpecificInt, void> and std::is_integral_v<T>) or
	                  same_as<T, SpecificInt>;

	template<typename T, typename SpecificInt = void>
	concept FloatingPoint =
	  (same_as<SpecificInt, void> and std::is_floating_point_v<T>) or
	  same_as<T, SpecificInt>;

	template<typename T, typename SpecificInt = void>
	concept Arithmetic =
	  (same_as<SpecificInt, void> and std::is_arithmetic_v<T>) or
	  same_as<T, SpecificInt>;

	template<typename From, typename To>
	concept constructible_to = constructible_from<To, From>;

	namespace concepts_details {
		template<typename, typename>
		inline constexpr bool Fn_v = false;

		template<typename F, typename R, typename... Args>
		inline constexpr bool Fn_v<F, R( Args... )> =
		  std::is_invocable_r_v<R, F, Args...>;
	} // namespace concepts_details

	template<typename F, typename ExpectedF>
	concept Fn = concepts_details::Fn_v<F, ExpectedF>;

	template<typename R, typename Fn, typename... Args>
	concept Callable_r = requires( Fn fn, Args... args ) {
		{ fn( args... ) } -> convertible_to<R>;
	};

	template<typename Fn, typename... Args>
	concept Callable = requires( Fn fn, Args... args ) {
		fn( args... );
	};

	template<typename R, typename Fn, typename... Args>
	concept NothrowCallable_r = requires( Fn fn, Args... args ) {
		{ fn( args... ) } noexcept -> convertible_to<R>;
	};

	template<typename Fn, typename... Args>
	concept NothrowCallable = requires( Fn fn, Args... args ) {
		fn( args... ) noexcept;
	};
} // namespace daw
// The MIT License (MIT)
//
// Copyright (c) 2014-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <deque>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cpp_17.h"

namespace daw {
	namespace traits {
		template<typename T>
		using root_type_t = std::decay_t<std::remove_reference_t<T>>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Return the largest sizeof in list of types.  Used to pad
		///				unions and small value optimization
		///
		template<typename... Args>
		struct max_sizeof;

		template<typename First>
		struct max_sizeof<First> {
			using type = First;
			static const size_t value = sizeof( type );
		};

		// the biggest of everything in Args and First
		template<typename First, typename... Args>
		struct max_sizeof<First, Args...> {
			using next = typename max_sizeof<Args...>::type;
			using type = typename std::conditional<sizeof( First ) >= sizeof( next ),
			                                       First, next>::type;
			static const size_t value = sizeof( type );
		};

		template<typename... Types>
		using max_sizeof_t = typename max_sizeof<Types...>::type;

		template<typename... Types>
		constexpr auto const max_sizeof_v = max_sizeof<Types...>::value;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Returns true if all values passed are true
		///
		template<typename BoolType,
		         typename std::enable_if<!is_integral_v<BoolType>, long>::type = 0>
		constexpr bool are_true( BoolType b1 ) noexcept {
			return b1 == true;
		}

		template<typename IntLike,
		         typename std::enable_if<is_integral_v<IntLike>, long>::type = 0>
		constexpr bool are_true( IntLike b1 ) noexcept {
			return b1 != 0;
		}

		template<typename BoolType1, typename BoolType2>
		constexpr bool are_true( BoolType1 b1, BoolType2 b2 ) noexcept {
			return are_true( b1 ) && are_true( b2 );
		}

		template<typename BoolType1, typename BoolType2, typename... Booleans>
		constexpr bool are_true( BoolType1 b1, BoolType2 b2,
		                         Booleans... others ) noexcept {
			return are_true( b1, b2 ) && are_true( others... );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Are all template parameters the same type
		///
		namespace impl {
			template<typename T, typename... Rest>
			struct are_same_types : std::false_type {};

			template<typename T, typename First>
			struct are_same_types<T, First> : std::is_same<T, First> {};

			template<typename T, typename First, typename... Rest>
			struct are_same_types<T, First, Rest...>
			  : std::integral_constant<bool, is_same_v<T, First> ||
			                                   are_same_types<T, Rest...>::value> {};
		} // namespace impl

		template<typename T, typename... Rest>
		constexpr bool are_same_types_v = impl::are_same_types<T, Rest...>::value;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	A sequence of bool values
		///
		template<bool...>
		struct bool_sequence {};

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Integral constant with result of and'ing all bool's supplied
		///
		template<bool... Bools>
		using bool_and = std::is_same<bool_sequence<Bools...>,
		                              bool_sequence<( Bools || true )...>>;

		template<bool... Bools>
		constexpr bool bool_and_v = bool_and<Bools...>::value;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Integral constant with result of or'ing all bool's supplied
		///
		template<bool... Bools>
		using bool_or =
		  std::integral_constant<bool, ( !bool_and<!Bools...>::value )>;

		template<bool... Bools>
		constexpr bool bool_or_v = bool_or<Bools...>::value;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Similar to enable_if but enabled when any of the
		///				parameters is true
		///
		template<typename R, bool... Bs>
		using enable_if_any = std::enable_if<bool_or_v<Bs...>, R>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Similar to enable_if but enabled when all of the
		///				parameters is true
		///
		template<typename R, bool... Bs>
		using enable_if_all = std::enable_if<bool_and_v<Bs...>, R>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Is type T on of the other types
		///
		namespace impl {
			template<typename T, typename... Types>
			struct is_one_of : public std::false_type {};

			template<typename T, typename Type>
			struct is_one_of<T, Type> : public std::is_same<T, Type> {};

			template<typename T, typename Type, typename... Types>
			struct is_one_of<T, Type, Types...>
			  : public std::integral_constant<bool, is_same_v<T, Type> ||
			                                          is_one_of<T, Types...>::value> {
			};

		} // namespace impl

		template<typename T, typename... Types>
		constexpr bool is_one_of_v = impl::is_one_of<T, Types...>::value;

		namespace details {
			template<typename>
			struct type_sink {
				using type = void;
			}; // consumes a type, and makes it `void`
			template<typename T>
			using type_sink_t = typename type_sink<T>::type;
		} // namespace details

// Build a check for the precence of a member
// Can be used like METHOD_CHECKER_ANY( has_begin_method, begin, ( ) ) and the
// check will look for begin( ) If you want to check args you could do the
// following METHOD_CHECKER_ANY( has_index_operation, operator[], (
// std::declval<size_t>( ) ) )
#define METHOD_CHECKER_ANY( name, fn, args )                                   \
	namespace impl {                                                             \
		template<typename T, typename = void>                                      \
		struct name : std::false_type {};                                          \
                                                                               \
		template<typename T>                                                       \
		struct name<T, typename std::enable_if_t<!is_same_v<                       \
		                 decltype( std::declval<T>( ).fn args ) *, void>>>         \
		  : std::true_type {};                                                     \
	}                                                                            \
                                                                               \
	template<typename T>                                                         \
	constexpr bool name##_v = impl::name<T>::value;
		  // END METHOD_CHECKER_ANY

		/*
		#define GENERATE_HAS_MEMBER_FUNCTION_TRAIT( MemberName ) \
		  namespace impl { \
		    template<typename T, typename = void> \
		    class has_##MemberName##_member_impl : public std::false_type {}; \
		                                                                                                                       \
		    template<typename T> \
		    class has_##MemberName##_member_impl<T, typename
		std::enable_if_t<is_class_v<T>>> { \
		      struct Fallback { \
		        int MemberName; \
		      }; \
		      struct Derived : T, Fallback {}; \
		                                                                                                                       \
		      template<typename U, U> \
		      struct Check; \
		                                                                                                                       \
		      using ArrayOfOne = char[1]; \
		      using ArrayOfTwo = char[2]; \
		                                                                                                                       \
		      template<typename U> \
		      static ArrayOfOne &func( Check<int Fallback::*, &U::MemberName> * ); \
		      template<typename U> \
		      static ArrayOfTwo &func( ... ); \
		                                                                                                                       \
		    public: \
		      using type = has_##MemberName##_member_impl; \
		      enum { value = sizeof( func<Derived>( 0 ) ) == 2 }; \
		    }; \
		  } \
		  template<typename T> \ using has_##MemberName##_member_v =
		::daw::traits::impl::has_##MemberName##_member_impl::value;
		*/

#define HAS_STATIC_TYPE_MEMBER( MemberName )                                   \
	namespace detectors {                                                        \
		template<typename T>                                                       \
		using MemberName##_member =                                                \
		  decltype( std::declval<typename T::MemberName>( ) );                     \
	}                                                                            \
	template<typename T>                                                         \
	constexpr bool has_##MemberName##_member_v =                                 \
	  is_detected_v<detectors::MemberName##_member, T>;

		HAS_STATIC_TYPE_MEMBER( type );
		HAS_STATIC_TYPE_MEMBER( value_type );
		HAS_STATIC_TYPE_MEMBER( mapped_type );
		HAS_STATIC_TYPE_MEMBER( iterator );

		METHOD_CHECKER_ANY( has_begin_member, begin, ( ) );
		METHOD_CHECKER_ANY( has_end_member, end, ( ) );
		METHOD_CHECKER_ANY( has_substr_member, substr, ( 0, 1 ) );

		template<typename T>
		constexpr bool is_string_v = all_true_v<is_convertible_v<T, std::string> ||
		                                        is_convertible_v<T, std::wstring>>;

		template<typename T>
		constexpr bool isnt_string_v = !is_string_v<T>;

		template<typename T>
		constexpr bool is_container_like_v =
		  all_true_v<has_begin_member_v<T>, has_end_member_v<T>>;

		template<typename T>
		constexpr bool is_container_not_string_v =
		  all_true_v<isnt_string_v<T>, is_container_like_v<T>>;

		template<typename T>
		constexpr bool is_map_like_v =
		  all_true_v<is_container_like_v<T>, has_mapped_type_member_v<T>>;

		template<typename T>
		constexpr bool isnt_map_like_v = !is_map_like_v<T>;

		template<typename T>
		constexpr bool is_vector_like_not_string_v =
		  all_true_v<is_container_not_string_v<T>, isnt_map_like_v<T>>;

		template<typename T>
		using static_not =
		  std::conditional<T::value, std::false_type, std::true_type>;

		template<typename T>
		using static_not_t = typename static_not<T>::type;

		template<typename T>
		constexpr bool static_not_v = static_not<T>::value;

#define GENERATE_IS_STD_CONTAINER1( ContainerName )                            \
	template<typename T>                                                         \
	constexpr bool is_##ContainerName##_v =                                      \
	  is_same_v<T, std::ContainerName<typename T::value_type>>;

		GENERATE_IS_STD_CONTAINER1( vector );
		GENERATE_IS_STD_CONTAINER1( list );
		GENERATE_IS_STD_CONTAINER1( set );
		GENERATE_IS_STD_CONTAINER1( unordered_set );
		GENERATE_IS_STD_CONTAINER1( deque );

#undef GENERATE_IS_STD_CONTAINER1

#define GENERATE_IS_STD_CONTAINER2( ContainerName )                            \
	template<typename T>                                                         \
	constexpr bool is_##ContainerName##_v = is_same_v<                           \
	  T, std::ContainerName<typename T::key_type, typename T::mapped_type>>;

		GENERATE_IS_STD_CONTAINER2( map );
		GENERATE_IS_STD_CONTAINER2( unordered_map );

#undef GENERATE_IS_STD_CONTAINER2

		template<typename T>
		constexpr bool is_single_item_container_v =
		  any_true_v<is_vector_v<T>, is_list_v<T>, is_set_v<T>, is_deque_v<T>,
		             is_unordered_set_v<T>>;

		template<typename T>
		constexpr bool is_container_v =
		  any_true_v<is_vector_v<T>, is_list_v<T>, is_set_v<T>, is_deque_v<T>,
		             is_unordered_set_v<T>, is_map_v<T>, is_unordered_map_v<T>>;

		template<typename T>
		constexpr bool is_map_type_v =
		  any_true_v<is_map_v<T>, is_unordered_map_v<T>>;

		template<typename T>
		constexpr bool is_numberic_v =
		  any_true_v<is_floating_point_v<T>, is_integral_v<T>>;

		template<typename T>
		constexpr bool is_container_or_array_v =
		  any_true_v<is_container_v<T>, is_array_v<T>>;

		namespace detectors {
			template<typename OutStream, typename T>
			using streamable =
			  decltype( std::declval<OutStream>( ) << std::declval<T>( ) );
		}
		template<typename OutStream, typename T>
		constexpr bool is_streamable_v =
		  daw::is_detected_v<detectors::streamable, OutStream &, T>;

		template<template<class> class Base, typename Derived>
		constexpr bool is_mixed_from_v = is_base_of_v<Base<Derived>, Derived>;

		template<std::size_t I, typename T, typename... Ts>
		struct nth_element_impl {
			using type = typename nth_element_impl<I - 1, Ts...>::type;
		};

		template<typename T, typename... Ts>
		struct nth_element_impl<0, T, Ts...> {
			using type = T;
		};

		template<std::size_t I, typename... Ts>
		using nth_element = typename nth_element_impl<I, Ts...>::type;

		namespace impl {
			template<typename T>
			auto has_to_string( T const &, long ) -> std::false_type;

			template<typename T>
			auto has_to_string( T const &lhs, int )
			  -> std::is_convertible<decltype( lhs.to_string( ) ), std::string>;
		} // namespace impl

		template<typename T>
		constexpr bool has_to_string_v =
		  decltype( impl::has_to_string( std::declval<T>( ), 1 ) )::value;

		namespace operators {
			namespace impl {
				template<typename L, typename R>
				auto has_op_eq_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_eq_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs == rhs ), bool>;

				template<typename L, typename R>
				auto has_op_ne_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_ne_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs != rhs ), bool>;

				template<typename L, typename R>
				auto has_op_lt_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_lt_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs < rhs ), bool>;

				template<typename L, typename R>
				auto has_op_le_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_le_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs <= rhs ), bool>;

				template<typename L, typename R>
				auto has_op_gt_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_gt_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs > rhs ), bool>;

				template<typename L, typename R>
				auto has_op_ge_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_ge_impl( L const &lhs, R const &rhs, int )
				  -> std::is_convertible<decltype( lhs >= rhs ), bool>;
			} // namespace impl

			template<typename L, typename R = L>
			constexpr bool has_op_eq_v = decltype( impl::has_op_eq_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

			template<typename L, typename R = L>
			constexpr bool has_op_ne_v = decltype( impl::has_op_ne_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

			template<typename L, typename R = L>
			constexpr bool has_op_lt_v = decltype( impl::has_op_lt_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

			template<typename L, typename R = L>
			constexpr bool has_op_le_v = decltype( impl::has_op_le_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

			template<typename L, typename R = L>
			constexpr bool has_op_gt_v = decltype( impl::has_op_gt_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

			template<typename L, typename R = L>
			constexpr bool has_op_ge_v = decltype( impl::has_op_ge_impl(
			  std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		} // namespace operators

		template<typename T, typename U>
		constexpr inline bool not_self( ) {
			using decayed_t = typename std::decay_t<T>;
			return !is_same_v<decayed_t, U> && !is_base_of_v<U, decayed_t>;
		}
	} // namespace traits

	template<typename T>
	using make_fp = std::add_pointer_t<T>;

	namespace detectors {
		template<typename Function, typename... Args>
		using callable_with =
		  decltype( std::declval<Function>( )( std::declval<Args>( )... ) );

		template<typename BinaryPredicate, typename T, typename U = T>
		using binary_predicate = callable_with<BinaryPredicate, T, U>;

		template<typename UnaryPredicate, typename T>
		using unary_predicate = callable_with<UnaryPredicate, T>;

		template<typename T, typename U>
		using equality_comparable =
		  decltype( std::declval<T>( ) == std::declval<U>( ) );

		template<typename T, typename U>
		using inequality_comparable =
		  decltype( std::declval<T>( ) != std::declval<U>( ) );

		template<typename T, typename U>
		using less_than_comparable =
		  decltype( std::declval<T>( ) < std::declval<U>( ) );

		template<typename T, typename U>
		using equal_less_than_comparable =
		  decltype( std::declval<T>( ) <= std::declval<U>( ) );

		template<typename T, typename U>
		using greater_than_comparable =
		  decltype( std::declval<T>( ) > std::declval<U>( ) );

		template<typename T, typename U>
		using equal_greater_than_comparable =
		  decltype( std::declval<T>( ) >= std::declval<U>( ) );

		namespace details {
			template<typename T, typename U>
			void swap( T &lhs, U &rhs ) {
				using std::swap;
				swap( lhs, rhs );
			}
		} // namespace details

		template<typename T>
		using swappable =
		  decltype( details::swap( std::declval<T>( ), std::declval<T>( ) ) );

		template<typename Iterator, typename T>
		using assignable =
		  decltype( *std::declval<Iterator>( ) = std::declval<T>( ) );

		template<typename T>
		using dereferenceable = decltype( *std::declval<T>( ) );
	} // namespace detectors

	namespace impl {
		template<typename...>
		struct is_single_void_arg_t : std::false_type {};

		template<>
		struct is_single_void_arg_t<void> : std::true_type {};
	} // namespace impl

	template<typename... Args>
	constexpr bool is_single_void_arg_v =
	  impl::is_single_void_arg_t<Args...>::value;

	template<typename Function, typename... Args>
	constexpr bool is_callable_v =
	  is_detected_v<detectors::callable_with, Function, Args...> ||
	  ( is_single_void_arg_v<Args...> &&
	    is_detected_v<detectors::callable_with, Function> );

	template<typename Result, typename Function, typename... Args>
	constexpr bool is_callable_convertible_v =
	  is_detected_convertible_v<Result, detectors::callable_with, Function,
	                            Args...>;

	namespace impl {
		template<typename Function, typename... Args,
		         std::enable_if_t<is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr bool is_nothrow_callable_test( ) noexcept {
			return noexcept( std::declval<Function>( )( std::declval<Args>( )... ) );
		}

		template<typename Function, typename... Args,
		         std::enable_if_t<!is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		constexpr bool is_nothrow_callable_test( ) noexcept {
			return false;
		}
	} // namespace impl

	template<typename Function, typename... Args>
	constexpr bool is_nothrow_callable_v =
	  impl::is_nothrow_callable_test<Function, Args...>( );

	namespace impl {
		template<typename Function>
		struct void_function {
			Function function;

			constexpr void_function( ) noexcept(
			  is_nothrow_constructible_v<Function> ) = default;

			explicit constexpr void_function( Function const &func ) noexcept(
			  is_nothrow_copy_constructible_v<Function> )
			  : function( func ) {}

			explicit constexpr void_function( Function &&func ) noexcept(
			  is_nothrow_move_constructible_v<Function> )
			  : function( std::move( func ) ) {}

			explicit constexpr operator bool( ) noexcept(
			  noexcept( static_cast<bool>( std::declval<Function>( ) ) ) ) {

				return static_cast<bool>( function );
			}

			template<typename... Args,
			         std::enable_if_t<daw::is_callable_v<Function, Args...>,
			                          std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&... args ) noexcept(
			  daw::is_nothrow_callable_v<Function, Args...> ) {
				function( std::forward<Args>( args )... );
			}
		};
	} // namespace impl

	template<typename Function>
	constexpr auto make_void_function( Function &&func ) noexcept( noexcept(
	  impl::void_function<Function>( std::forward<Function>( func ) ) ) ) {

		return impl::void_function<Function>( std::forward<Function>( func ) );
	}

	template<typename T>
	using is_dereferenceable_t =
	  typename is_detected<detectors::dereferenceable, T>::type;

	template<typename T>
	constexpr bool is_dereferenceable_v =
	  is_detected_v<detectors::dereferenceable, T>;

	template<typename Function, typename... Args>
	using is_callable_t =
	  typename is_detected<detectors::callable_with, Function, Args...>::type;

	template<typename Predicate, typename... Args>
	constexpr bool is_predicate_v =
	  is_detected_convertible_v<bool, detectors::callable_with, Predicate,
	                            Args...>;

	template<typename BinaryPredicate, typename T, typename U = T>
	constexpr bool is_binary_predicate_v = is_predicate_v<BinaryPredicate, T, U>;

	template<typename UnaryPredicate, typename T>
	constexpr bool is_unary_predicate_v = is_predicate_v<UnaryPredicate, T>;

	template<typename Compare, typename T, typename U = T>
	constexpr bool is_compare_v = is_binary_predicate_v<Compare, T, U>;

	template<typename T, typename U = T>
	constexpr bool is_equality_comparable_v =
	  is_detected_convertible_v<bool, detectors::equality_comparable, T, U>;

	template<typename T, typename U = T>
	constexpr bool is_inequality_comparable_v =
	  is_detected_convertible_v<bool, detectors::inequality_comparable, T, U>;

	template<typename T, typename U = T>
	constexpr bool is_less_than_comparable_v =
	  is_detected_convertible_v<bool, detectors::less_than_comparable, T, U>;

	template<typename T, typename U = T>
	constexpr bool is_equal_less_than_comparable_v =
	  is_detected_convertible_v<bool, detectors::equal_less_than_comparable, T,
	                            U>;

	template<typename T, typename U = T>
	constexpr bool is_greater_than_comparable_v =
	  is_detected_convertible_v<bool, detectors::greater_than_comparable, T, U>;

	template<typename T, typename U = T>
	constexpr bool is_equal_greater_than_comparable_v =
	  is_detected_convertible_v<bool, detectors::equal_greater_than_comparable, T,
	                            U>;

	template<typename Iterator,
	         typename T = typename std::iterator_traits<Iterator>::value_type>
	constexpr bool is_assignable_iterator_v =
	  is_detected_v<detectors::assignable, Iterator, T>;

	template<typename L, typename R>
	constexpr bool is_comparable_v =
	  is_equality_comparable_v<L, R> &&is_equality_comparable_v<R, L>;

	namespace impl {
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

			template<typename T>
			using is_incrementable = decltype( ++std::declval<T &>( ) );
		} // namespace is_iter
		template<typename T>
		constexpr bool is_incrementable_v =
		  is_same_v<T &, daw::detected_t<is_iter::is_incrementable, T>>;

		template<typename T>
		constexpr bool has_value_type_v =
		  daw::is_detected_v<is_iter::has_value_type, T>;

		template<typename T>
		constexpr bool has_difference_type_v =
		  daw::is_detected_v<is_iter::has_difference_type, T>;

		template<typename T>
		constexpr bool has_reference_v =
		  daw::is_detected_v<is_iter::has_reference, T>;

		template<typename T>
		constexpr bool has_pointer_v = daw::is_detected_v<is_iter::has_pointer, T>;

		template<typename T>
		constexpr bool has_iterator_category_v =
		  daw::is_detected_v<is_iter::has_iterator_category, T>;

		template<typename T>
		constexpr bool has_iterator_trait_types_v =
		  has_value_type_v<T> &&has_difference_type_v<T> &&has_reference_v<T>
		    &&has_pointer_v<T> &&has_iterator_category_v<T>;
	} // namespace impl

	// TODO: add is_swappable after c++17. Cannot do in C++14
	template<typename Iterator>
	constexpr bool is_iterator_v =
	  is_copy_constructible_v<Iterator> &&is_copy_assignable_v<Iterator>
	    &&is_destructible_v<Iterator> &&impl::has_iterator_trait_types_v<Iterator>
	      &&is_dereferenceable_v<Iterator> &&impl::is_incrementable_v<Iterator>;

	template<typename OutputIterator, typename T>
	constexpr bool is_output_iterator_v =
	  is_iterator_v<OutputIterator> &&is_assignable_iterator_v<OutputIterator, T>;

	template<typename InputIterator,
	         typename T =
	           std::decay_t<decltype( *std::declval<InputIterator>( ) )>>
	constexpr bool is_input_iterator_v =
	  is_iterator_v<InputIterator> &&is_equality_comparable_v<InputIterator>
	    &&is_convertible_v<decltype( *std::declval<InputIterator>( ) ), T>;

	template<typename ForwardIterator,
	         typename T =
	           std::decay_t<decltype( *std::declval<ForwardIterator>( ) )>>
	constexpr bool is_forward_iterator_v = is_input_iterator_v<ForwardIterator, T>
	  &&is_default_constructible_v<ForwardIterator>;

	//////////////////////////////////////////////////////////////////////////
	/// Summary: is like a regular type see
	/// http://www.stepanovpapers.com/DeSt98.pdf
	/// Now using C++'s defition of Regular
	/// http://en.cppreference.com/w/cpp/experimental/ranges/concepts/Regular
	template<typename T>
	constexpr bool is_regular_v =
	  all_true_v<is_default_constructible_v<T>, is_copy_constructible_v<T>,
	             is_move_constructible_v<T>, is_copy_assignable_v<T>,
	             is_move_assignable_v<T>, is_equality_comparable_v<T>,
	             is_inequality_comparable_v<T>>;

	struct nothing {};

	template<typename To, typename... From>
	constexpr bool are_convertible_to_v =
	  all_true_v<is_convertible_v<From, To>...>;

	template<size_t N, typename... Args>
	using type_n_t = std::tuple_element_t<N, std::tuple<Args...>>;

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

	namespace traits {
		namespace detectors {
			template<typename T>
			using has_integer_subscript = decltype( std::declval<T>( )[0] );

			template<typename T>
			using has_size =
			  decltype( std::declval<size_t &>( ) = std::declval<T>( ).size( ) );

			template<typename T>
			using is_array_array = decltype( std::declval<T>( )[0][0] );

			template<typename T>
			using has_empty =
			  decltype( std::declval<bool &>( ) = std::declval<T>( ).empty( ) );

			template<typename T>
			using has_append_operator = decltype(
			  std::declval<T &>( ) += std::declval<has_integer_subscript<T>>( ) );

			template<typename T>
			using has_append =
			  decltype( std::declval<T>( ).append( std::declval<T>( ) ) );

			template<typename T, typename U>
			using has_addition_operator =
			  decltype( std::declval<T>( ) + std::declval<U>( ) );

			template<typename T, typename U>
			using has_subtraction_operator =
			  decltype( std::declval<T>( ) - std::declval<U>( ) );

			template<typename T, typename U>
			using has_multiplication_operator =
			  decltype( std::declval<T>( ) * std::declval<U>( ) );

			template<typename T, typename U>
			using has_division_operator =
			  decltype( std::declval<T>( ) / std::declval<U>( ) );

			template<typename T, typename U>
			using has_modulus_operator =
			  decltype( std::declval<T>( ) % std::declval<U>( ) );

			template<typename T>
			using has_increment_operator = decltype( ++std::declval<T &>( ) );

			template<typename T>
			using has_decrement_operator = decltype( --std::declval<T &>( ) );
		} // namespace detectors

		template<typename T, typename U = T>
		constexpr bool has_addition_operator_v =
		  daw::is_detected_v<detectors::has_addition_operator, T, U>;

		template<typename T, typename U = T>
		constexpr bool has_subtraction_operator_v =
		  daw::is_detected_v<detectors::has_subtraction_operator, T, U>;

		template<typename T, typename U = T>
		constexpr bool has_multiplication_operator_v =
		  daw::is_detected_v<detectors::has_multiplication_operator, T, U>;

		template<typename T, typename U = T>
		constexpr bool has_division_operator_v =
		  daw::is_detected_v<detectors::has_division_operator, T, U>;

		template<typename T, typename U = T>
		constexpr bool has_modulus_operator_v =
		  daw::is_detected_v<detectors::has_modulus_operator, T, U>;

		template<typename T>
		constexpr bool has_increment_operator_v =
		  daw::is_detected_v<detectors::has_increment_operator, T>;

		template<typename T>
		constexpr bool has_decrement_operator_v =
		  daw::is_detected_v<detectors::has_decrement_operator, T>;

		template<typename String>
		constexpr bool has_integer_subscript_v =
		  daw::is_detected_v<detectors::has_integer_subscript, String>;

		template<typename String>
		constexpr bool has_size_memberfn_v =
		  daw::is_detected_v<detectors::has_size, String>;

		template<typename String>
		constexpr bool has_empty_memberfn_v =
		  daw::is_detected_v<detectors::has_empty, String>;

		template<typename String>
		constexpr bool has_append_memberfn_v =
		  daw::is_detected_v<detectors::has_append, String>;

		template<typename String>
		constexpr bool has_append_operator_v =
		  daw::is_detected_v<detectors::has_append_operator, String>;

		template<typename String>
		constexpr bool is_not_array_array_v =
		  !daw::is_detected_v<detectors::is_array_array, String>;

		template<typename String>
		constexpr bool is_string_view_like_v = is_container_like_v<String const> &&
		  has_integer_subscript_v<String const> &&has_size_memberfn_v<String const>
		    &&has_empty_memberfn_v<String const> &&is_not_array_array_v<String>;

		template<typename String>
		constexpr bool is_string_like_v = is_string_view_like_v<String>
		  &&has_append_operator_v<String> &&has_append_memberfn_v<String>
		    &&is_container_like_v<String> &&has_integer_subscript_v<String>;

		template<typename Container, size_t ExpectedSize>
		constexpr bool is_value_size_equal_v =
		  sizeof( *std::cbegin( std::declval<Container>( ) ) ) == ExpectedSize;

		namespace impl {
			template<typename... Ts>
			struct are_unique;

			template<typename T1>
			struct are_unique<T1> : std::true_type {};

			template<typename T1, typename T2>
			struct are_unique<T1, T2>
			  : std::integral_constant<bool, !daw::is_same_v<T1, T2>> {};

			template<typename T1, typename T2, typename... Ts>
			struct are_unique<T1, T2, Ts...>
			  : std::integral_constant<bool, are_unique<T1, T2>::value &&
			                                   are_unique<T1, Ts...>::value &&
			                                   are_unique<T2, Ts...>::value> {};
		} // namespace impl

		template<typename... Ts>
		using are_unique_t = impl::are_unique<Ts...>;
		template<typename... Ts>
		constexpr bool are_unique_v = are_unique_t<Ts...>::value;

		namespace impl {
			template<typename...>
			struct isnt_cv_ref;

			template<>
			struct isnt_cv_ref<> : std::true_type {};

			template<typename T, typename... Ts>
			struct isnt_cv_ref<T, Ts...>
			  : std::integral_constant<
			      bool, ( !(is_const_v<T> || is_reference_v<T> ||
			                is_volatile_v<
			                  T>)&&is_same_v<std::true_type,
			                                 typename isnt_cv_ref<Ts...>::type> )> {};
		} // namespace impl

		template<typename... Ts>
		constexpr bool isnt_cv_ref_v = impl::isnt_cv_ref<Ts...>::value;

		template<typename T>
		using deref_t = decltype( *std::declval<T>( ) );

		template<typename Function, typename... Args>
		using result_of_t =
		  decltype( std::declval<Function>( )( std::declval<Args>( )... ) );

		namespace impl {
			template<typename T, typename... Args>
			struct first_type_impl {
				using type = T;
			};
		} // namespace impl

		template<typename... Args>
		using first_type = typename impl::first_type_impl<Args...>::type;

		namespace impl {
			template<typename... Ts>
			constexpr void tuple_test( std::tuple<Ts...> const & ) noexcept {}

			template<typename... Ts>
			using detect_is_tuple = decltype( tuple_test( std::declval<Ts>( )... ) );
		} // namespace impl

		template<typename...>
		struct is_first_type;

		template<typename T>
		struct is_first_type<T> : std::false_type {};

		template<typename T, typename Arg, typename... Args>
		struct is_first_type<T, Arg, Args...> : std::is_same<T, std::decay_t<Arg>> {
		};

		// Tests if type T is the same as the first argument in Args or if args is
		// empty it is false
		template<typename T, typename... Args>
		constexpr bool is_first_type_v = is_first_type<T, Args...>::value;

		template<typename... Ts>
		constexpr bool is_tuple_v = is_detected_v<impl::detect_is_tuple, Ts...>;

		namespace impl {}
		template<typename T, typename... Args>
		constexpr bool is_init_list_constructible_v = are_same_types_v<Args...>
		  &&daw::is_constructible_v<T, std::initializer_list<first_type<Args...>>>;

		namespace ostream_detectors {
			template<typename T>
			using has_char_type_detect = typename T::char_type;

			template<typename T>
			constexpr bool has_char_type_v =
			  daw::is_detected_v<has_char_type_detect, T>;

			template<typename T>
			using has_adjustfield_detect = decltype( T::adjustfield );

			template<typename T>
			constexpr bool has_adjustfield_v =
			  daw::is_detected_v<has_adjustfield_detect, T>;

			template<typename T>
			using has_left_detect = decltype( T::left );

			template<typename T>
			constexpr bool has_left_v = daw::is_detected_v<has_left_detect, T>;

			template<typename T>
			using has_fill_member_detect = decltype( std::declval<T>( ).fill( ) );

			template<typename T>
			constexpr bool has_fill_member_v =
			  daw::is_detected_v<has_fill_member_detect, T>;

			template<typename T>
			using has_good_member_detect = decltype( std::declval<T>( ).good( ) );

			template<typename T>
			constexpr bool has_good_member_v =
			  daw::is_detected_v<has_good_member_detect, T>;

			template<typename T, typename CharT>
			using has_write_member_detect = decltype( std::declval<T>( ).write(
			  std::declval<CharT const *>( ), std::declval<int>( ) ) );

			template<typename T, typename CharT>
			constexpr bool has_write_member_v =
			  daw::is_detected_v<has_write_member_detect, T, CharT>;

			template<typename T>
			using has_width_member_detect = decltype( std::declval<T>( ).width( ) );

			template<typename T>
			constexpr bool has_width_member_v =
			  daw::is_detected_v<has_width_member_detect, T>;

			template<typename T>
			using has_flags_member_detect = decltype( std::declval<T>( ).flags( ) );

			template<typename T>
			constexpr bool has_flags_member_v =
			  daw::is_detected_v<has_flags_member_detect, T>;

		} // namespace ostream_detectors

		template<typename T>
		constexpr bool is_ostream_like_lite_v =
		  ostream_detectors::has_char_type_v<T>
		    &&ostream_detectors::has_adjustfield_v<T>
		      &&ostream_detectors::has_fill_member_v<T>
		        &&ostream_detectors::has_good_member_v<T>
		          &&ostream_detectors::has_width_member_v<T>
		            &&ostream_detectors::has_flags_member_v<T>;

		template<typename T, typename CharT>
		constexpr bool is_ostream_like_v = is_ostream_like_lite_v<T>
		  &&ostream_detectors::has_write_member_v<T, CharT>;
	} // namespace traits

	template<bool B, typename T = std::nullptr_t>
	using required = std::enable_if_t<B, T>;

	template<size_t N, typename... Args>
	struct pack_type {
		using type = std::decay_t<std::tuple_element_t<N, std::tuple<Args...>>>;
	};

	template<size_t N, typename... Args>
	using pack_type_t = typename pack_type<N, Args...>::type;

	namespace impl {
		struct non_constructor {};

		template<bool>
		struct delete_default_constructor_if {
			constexpr delete_default_constructor_if( ) noexcept {}
			constexpr delete_default_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_default_constructor_if<true> {
			delete_default_constructor_if( ) = delete;

			~delete_default_constructor_if( ) noexcept = default;
			constexpr delete_default_constructor_if( non_constructor ) noexcept {}
			constexpr delete_default_constructor_if(
			  delete_default_constructor_if const & ) {}
			constexpr delete_default_constructor_if &
			operator=( delete_default_constructor_if const & ) noexcept {
				return *this;
			}
			constexpr delete_default_constructor_if(
			  delete_default_constructor_if && ) noexcept {}
			constexpr delete_default_constructor_if &
			operator=( delete_default_constructor_if && ) noexcept {
				return *this;
			}
		};

		template<bool>
		struct delete_copy_constructor_if {
			constexpr delete_copy_constructor_if( ) noexcept {}
			constexpr delete_copy_constructor_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_copy_constructor_if<true> {
			delete_copy_constructor_if( delete_copy_constructor_if const & ) = delete;

			constexpr delete_copy_constructor_if( non_constructor ) noexcept {}
			constexpr delete_copy_constructor_if( ) noexcept {}
			~delete_copy_constructor_if( ) noexcept = default;
			constexpr delete_copy_constructor_if &
			operator=( delete_copy_constructor_if const & ) noexcept {
				return *this;
			}
			constexpr delete_copy_constructor_if(
			  delete_copy_constructor_if && ) noexcept {}
			constexpr delete_copy_constructor_if &
			operator=( delete_copy_constructor_if && ) noexcept {
				return *this;
			}
		};

		template<bool>
		struct delete_copy_assignment_if {
			constexpr delete_copy_assignment_if( ) noexcept {}
			constexpr delete_copy_assignment_if( non_constructor ) noexcept {}
		};

		template<>
		struct delete_copy_assignment_if<true> {
			delete_copy_assignment_if &
			operator=( delete_copy_assignment_if const & ) = delete;

			constexpr delete_copy_assignment_if( non_constructor ) noexcept {}
			constexpr delete_copy_assignment_if( ) noexcept {}
			~delete_copy_assignment_if( ) noexcept = default;
			constexpr delete_copy_assignment_if(
			  delete_copy_assignment_if const & ) noexcept {}
			constexpr delete_copy_assignment_if(
			  delete_copy_assignment_if && ) noexcept {}
			constexpr delete_copy_assignment_if &
			operator=( delete_copy_assignment_if && ) noexcept {
				return *this;
			}
		};
	} // namespace impl

	template<typename T>
	using enable_default_constructor =
	  impl::delete_default_constructor_if<!is_default_constructible_v<T>>;

	template<typename T>
	using enable_copy_constructor =
	  impl::delete_copy_constructor_if<!is_copy_constructible_v<T>>;

	template<typename T>
	using enable_copy_assignment =
	  impl::delete_copy_assignment_if<!is_copy_assignable_v<T>>;
} // namespace daw

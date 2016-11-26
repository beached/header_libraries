// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/type_traits.hpp>
#include <type_traits>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace daw {
	namespace traits {
		template<typename T>
			using root_type_t = std::decay_t<std::remove_reference_t<T>>;

		template<typename ...>
			using void_t = void;

		namespace details {
			template<typename T>
			std::false_type is_equality_comparable_impl( T const &, long );

			template<typename T>
			auto is_equality_comparable_impl( T const & value, int ) -> typename std::is_convertible<decltype(value == value), bool>;
		}	// namespace details

		template<typename T>
		struct is_equality_comparable: decltype(details::is_equality_comparable_impl(std::declval<T const &>( ), 1 )) {};

		namespace impl {
			template<class ...>
				using void_t = void;

			template<typename L, typename R>
				using comparability = decltype( (std::declval<root_type_t<R>>( ) == std::declval<root_type_t<L>>( )) && (std::declval<root_type_t<L>>( ) == std::declval<root_type_t<R>>( )) );

		}

		template<typename L, typename R, class = void>
			struct is_comparable: std::false_type { };


		template<typename L, typename R>
			struct is_comparable<L, R, impl::void_t<impl::comparability<L, R>>>: std::true_type { };

		template<typename L, typename R>
			constexpr auto is_comparable_v = is_comparable<L, R>::value;

		template<typename L, typename R>
			using is_comparable_t = typename is_comparable<L, R>::type;

		//////////////////////////////////////////////////////////////////////////
		/// Summary: is like a regular type see http://www.stepanovpapers.com/DeSt98.pdf
		template<typename T>
			using is_regular =std::integral_constant < bool,std::is_default_constructible<T>::value &&
			std::is_copy_constructible<T>::value &&std::is_move_constructible<T>::value &&
			std::is_copy_assignable<T>::value &&std::is_move_assignable<T>::value &&
			is_equality_comparable<T>::value>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Return the largest sizeof in list of types.  Used to pad
		///				unions and small value optimization
		///
		template <typename... Args>
			struct max_sizeof;

		template <typename First>
			struct max_sizeof<First> {
				using type = First;
				static const size_t value = sizeof( type );
			};

		//the biggest of everything in Args and First
		template<typename First, typename... Args>
			struct max_sizeof< First, Args...> {
				using next = typename max_sizeof<Args...>::type;
				using type = typename std::conditional<sizeof( First ) >= sizeof( next ), First, next>::type;
				static const size_t value = sizeof( type );
			};

		template<typename... Types>
			using max_sizeof_t = typename max_sizeof<Types...>::type;

		template<typename... Types>
			constexpr auto const max_sizeof_v = max_sizeof<Types...>::value;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Returns true if all values passed are true
		///
		template<typename BoolType, typename std::enable_if<!std::is_integral<BoolType>::value, long>::type = 0>
			bool are_true( BoolType b1 ) {
				return b1 == true;
			}

		template<typename IntLike, typename std::enable_if<std::is_integral<IntLike>::value, long>::type = 0>
			bool are_true( IntLike b1 ) {
				return b1 != 0;
			}

		template<typename BoolType1, typename BoolType2>
			bool are_true( BoolType1 b1, BoolType2 b2 ) {
				return are_true( b1 ) && are_true( b2 );
			}

		template<typename BoolType1, typename BoolType2, typename ...Booleans>
			bool are_true( BoolType1 b1, BoolType2 b2, Booleans... others ) {
				return are_true( b1, b2 ) && are_true( others... );
			}

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Are all template parameters the same type
		///
		template<typename T, typename... Rest>
			struct are_same_types: std::false_type { };

		template<typename T, typename First>
			struct are_same_types<T, First>: std::is_same < T, First> { };

		template<typename T, typename First, typename... Rest>
			struct are_same_types<T, First, Rest...>
			: std::integral_constant < bool,std::is_same<T, First>::value || are_same_types<T, Rest...>::value> { };

		template<typename T, typename... Rest>
			using are_same_types_t = typename are_same_types<T, Rest...>::type;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	A sequence of bool values
		///
		template<bool...> struct bool_sequence { };

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Integral constant with result of and'ing all bool's supplied
		///
		template<bool... Bools>
			using bool_and =std::is_same < bool_sequence< Bools...>,
				  bool_sequence< (Bools || true)...>>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Integral constant with result of or'ing all bool's supplied
		///
		template<bool... Bools>
			using bool_or =std::integral_constant < bool, (!bool_and< !Bools...>::value)>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Similar to enable_if but enabled when any of the
		///				parameters is true
		///
		template< typename R, bool... Bs>
			using enable_if_any =std::enable_if < bool_or< Bs...>::value, R>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Similar to enable_if but enabled when all of the
		///				parameters is true
		///
		template< typename R, bool... Bs>
			using enable_if_all =std::enable_if < bool_and< Bs...>::value, R>;

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Is type T on of the other types
		///
		template<typename T, typename... Types>
			struct is_one_of: public std::false_type { };

		template<typename T, typename Type>
			struct is_one_of<T, Type>: public std::is_same < T, Type> { };

		template<typename T, typename Type, typename... Types>
			struct is_one_of<T, Type, Types...>: public std::integral_constant<bool,std::is_same<T, Type>::value || is_one_of<T, Types...>::value> { };

		template<typename... Types>
			using is_one_of_t = typename is_one_of<Types...>::type;

		template<typename... Types>
			constexpr auto const is_one_of_v = is_one_of<Types...>::value;

		namespace details {
			template<typename> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
			template<typename T> using type_sink_t = typename type_sink<T>::type;
		}

#define GENERATE_HAS_MEMBER_FUNCTION_TRAIT(MemberName) \
		namespace impl { \
			template<typename T, typename = void> \
			class has_##MemberName##_member_impl: public std::false_type {}; \
			template<typename T> \
			class has_##MemberName##_member_impl< T, typename std::enable_if<std::is_class<T>::value>::type> { \
				struct Fallback { \
					int MemberName; \
				}; \
				struct Derived : T, Fallback { }; \
				\
				template<typename U, U> struct Check; \
				\
				using ArrayOfOne = char[1]; \
				using ArrayOfTwo = char[2]; \
				\
				template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::MemberName> *); \
				template<typename U> static ArrayOfTwo & func(...); \
				public: \
						typedef has_##MemberName##_member_impl type; \
				enum { value = sizeof(func<Derived>(0)) == 2 }; \
			}; /*struct has_##MemberName##_member_impl*/ \
		} /* namespace impl */ \
		template<typename T> using has_##MemberName##_member =std::integral_constant<bool, impl::has_##MemberName##_member_impl<T>::value>; \
		template<typename T> using has_##MemberName##_member_v = typename has_##MemberName##_member<T>::value;	\
		template<typename T> using has_##MemberName##_member_t = typename has_##MemberName##_member<T>::type;

#define GENERATE_HAS_MEMBER_TYPE_TRAIT( TypeName ) \
		template<typename T, typename=void> struct has_##TypeName##_member: std::false_type { }; \
		template<typename T> struct has_##TypeName##_member<T, details::type_sink_t<typename T::TypeName>>: std::true_type { }; \
		template<typename T> using has_##TypeName##_member_v = typename has_##TypeName##_member<T>::value;	\
		template<typename T> using has_##TypeName##_member_t = typename has_##TypeName##_member<T>::type;

		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( begin );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( end );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( substr );
		GENERATE_HAS_MEMBER_FUNCTION_TRAIT( push_back );

		GENERATE_HAS_MEMBER_TYPE_TRAIT( type );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( value_type );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( mapped_type );
		GENERATE_HAS_MEMBER_TYPE_TRAIT( iterator );

		template<typename T> using is_string =std::is_convertible < T,std::string>;
		template<typename T> using is_string_t = typename is_string<T>::type;

		template<typename T> using isnt_string =std::integral_constant < bool, !is_string<T>::value>;
		template<typename T> using isnt_string_t = typename isnt_string<T>::type;

		template<typename T> using is_container_like =std::integral_constant < bool, has_begin_member<T>::value && has_end_member<T>::value>;
		template<typename T> using is_container_like_t = typename is_container_like<T>::type;
		template<typename T> constexpr bool is_container_like_v = is_container_like<T>::value;

		template<typename T> using is_container_not_string =std::integral_constant < bool, isnt_string<T>::value && is_container_like<T>::value>;
		template<typename T> using is_container_not_string_t = typename is_container_not_string<T>::type;

		template<typename T> using is_map_like =std::integral_constant < bool, is_container_like<T>::value && has_mapped_type_member<T>::value>;
		template<typename T> using is_map_like_t = typename is_map_like<T>::type;

		template<typename T> using isnt_map_like =std::integral_constant < bool, !is_map_like<T>::value>;
		template<typename T> using isnt_map_like_t = typename isnt_map_like<T>::type;

		template<typename T> using is_vector_like_not_string =std::integral_constant < bool, is_container_not_string<T>::value && isnt_map_like<T>::value>;
		template<typename T> using is_vector_like_not_string_t = typename is_vector_like_not_string<T>::type;

		template <typename T> using static_not =std::conditional < T::value,std::false_type,std::true_type>;

#define GENERATE_IS_STD_CONTAINER1( ContainerName ) \
		template<typename T> using is_##ContainerName =std::integral_constant<bool,std::is_same<T,std::ContainerName<typename T::value_type>>::value>; \
		template<typename T> using is_##ContainerName##_t = typename is_##ContainerName <T>::type;

		GENERATE_IS_STD_CONTAINER1( vector );
		GENERATE_IS_STD_CONTAINER1( list );
		GENERATE_IS_STD_CONTAINER1( set );
		GENERATE_IS_STD_CONTAINER1( unordered_set );
		GENERATE_IS_STD_CONTAINER1( deque );

#undef GENERATE_IS_STD_CONTAINER1

#define GENERATE_IS_STD_CONTAINER2( ContainerName ) \
		template<typename T> using is_##ContainerName =std::integral_constant<bool,std::is_same<T,std::ContainerName<typename T::key_type, typename T::mapped_type>>::value>; \
		template<typename T> using is_##ContainerName##_t = typename is_##ContainerName <T>::type;

		GENERATE_IS_STD_CONTAINER2( map );
		GENERATE_IS_STD_CONTAINER2( unordered_map );

#undef GENERATE_IS_STD_CONTAINER2

		template<typename T> using is_single_item_container =std::integral_constant < bool, is_vector<T>::value ||
			is_list<T>::value || is_set<T>::value || is_deque<T>::value || is_unordered_set<T>::value>;

		template<typename T> using is_single_item_container_t = typename is_single_item_container<T>::type;

		template<typename T> using is_container =std::integral_constant < bool, is_vector<T>::value || is_list<T>::value ||
			is_set<T>::value || is_deque<T>::value || is_unordered_set<T>::value || is_map<T>::value || is_unordered_map<T>::value>;

		template<typename T> using is_container_t = typename is_container<T>::type;

		template<typename T> using is_map_type =std::integral_constant < bool, is_map<T>::value || is_unordered_map<T>::value>;
		template<typename T> using is_map_type_t = typename is_map_type<T>::type;

		template<typename T> using is_numeric =std::integral_constant < bool,std::is_floating_point<T>::value ||std::is_integral<T>::value>;
		template<typename T> using is_numeric_t = typename is_numeric<T>::type;

		template<typename T> using is_container_or_array =std::integral_constant < bool, is_container<T>::value ||std::is_array<T>::value>;

		template<typename T> using is_container_or_array_t = typename is_container_or_array<T>::type;

		template<typename T>
			using is_streamable =std::integral_constant < bool, boost::has_left_shift<std::ostream&, T const &,std::ostream&>::value>;

		template<typename T>
			using is_streamable_t = typename is_streamable<T>::type;

		template<template<class> class Base, typename Derived>
			using is_mixed_from =std::integral_constant < bool,std::is_base_of<Base<Derived>, Derived>::value>;

		template<template<class> class Base, typename Derived>
			using is_mixed_from_t = typename is_mixed_from<Base, Derived>::type;

		template<typename T>
			constexpr bool is_integral_v = std::is_integral<T>::value;

		namespace impl {
			template<typename F, typename...Args>
				struct is_callable_impl {
					template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
					template<class U> static auto test(...) -> decltype(std::false_type());

					static constexpr bool value = decltype(test<F>(0))::value;
				};	// is_callable
		}	// namespace impl

		template<typename F, typename...Args>
			using is_callable = std::integral_constant<bool, impl::is_callable_impl<F, Args...>::value>;

		template<typename F, typename...Args>
			using is_callable_t = typename is_callable<F, Args...>::type;

		template<typename F, typename...Args>
			constexpr auto is_callable_v = impl::is_callable_impl<F, Args...>::value;

		template<std::size_t I, typename T, typename ...Ts>
		struct nth_element_impl {
			using type = typename nth_element_impl<I-1, Ts...>::type;
		};

		template <typename T, typename ...Ts>
		struct nth_element_impl<0, T, Ts...> {
			using type = T;
		};

		template <std::size_t I, typename ...Ts>
		using nth_element = typename nth_element_impl<I, Ts...>::type;

		namespace impl {
			template<typename T>
			auto has_to_string( T const &, long ) -> std::false_type;

			template<typename T>
			auto has_to_string( T const & lhs, int ) -> std::is_convertible<decltype(lhs.to_string( )), std::string>;
		}
		template<typename T>
		struct has_to_string: decltype(impl::has_to_string( std::declval<T>( ), 1 ) ) { };

		namespace operators {
			namespace impl {
				template<typename L, typename R>
				auto has_op_eq_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_eq_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs == rhs), bool>;

				template<typename L, typename R>
				auto has_op_ne_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_ne_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs != rhs), bool>;

				template<typename L, typename R>
				auto has_op_lt_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_lt_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs < rhs), bool>;

				template<typename L, typename R>
				auto has_op_le_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_le_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs <= rhs), bool>;

				template<typename L, typename R>
				auto has_op_gt_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_gt_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs > rhs), bool>;

				template<typename L, typename R>
				auto has_op_ge_impl( L const &, R const &, long ) -> std::false_type;

				template<typename L, typename R>
				auto has_op_ge_impl( L const & lhs, R const & rhs, int ) -> std::is_convertible<decltype(lhs >= rhs), bool>;
			}	// namespace impl

			template<typename L, typename R = L>
			struct has_op_eq: decltype(impl::has_op_eq_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

			template<typename L, typename R = L>
			struct has_op_ne: decltype(impl::has_op_ne_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

			template<typename L, typename R = L>
			struct has_op_lt: decltype(impl::has_op_lt_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

			template<typename L, typename R = L>
			struct has_op_le: decltype(impl::has_op_le_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

			template<typename L, typename R = L>
			struct has_op_gt: decltype(impl::has_op_gt_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

			template<typename L, typename R = L>
			struct has_op_ge: decltype(impl::has_op_ge_impl( std::declval<L>( ), std::declval<R>( ), 1 ) ) { };

		}	// namespace operators

		template <typename T, typename U>
		constexpr inline bool not_self( ) {
			using decayed_t = typename std::decay_t<T>;
		    return !std::is_same<decayed_t, U>::value && !std::is_base_of<U, decayed_t>::value;
		}
	}	// namespace traits
}	// namespace daw


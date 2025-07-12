// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_17.h"
#include "daw/daw_bit_count.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_enable_if.h"
#include "daw/daw_move.h"
#include "daw/daw_pack_element.h"
#include "daw/daw_remove_cvref.h"
#include "daw/impl/daw_make_trait.h"
#include "daw/impl/daw_traits_impl.h"
#include "daw/traits/daw_traits_conditional.h"
#include "daw/traits/daw_traits_first_type.h"
#include "daw/traits/daw_traits_is_const.h"
#include "daw/traits/daw_traits_is_one_of.h"
#include "daw/traits/daw_traits_is_rvalue_reference.h"

#include <cstdlib>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/range_access.h>
#include <tuple>
#include <type_traits>

namespace daw::traits {
	template<typename T, typename... Ts>
	inline constexpr bool all_same_v = ( std::is_same_v<T, Ts> and ... );

	template<typename T>
	using root_type_t = std::decay_t<std::remove_reference_t<T>>;

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Return the largest sizeof in list of types.  Used to pad
	///				unions and small value optimization
	///
	template<typename...>
	struct max_sizeof;

	template<typename First>
	struct max_sizeof<First> {
		using type = First;
		static const std::size_t value = sizeof( type );
	};

	template<typename First, typename Second>
	struct max_sizeof<First, Second> {
		using type = std::conditional_t<( sizeof( First ) >= sizeof( Second ) ),
		                                First, Second>;
		static constexpr std::size_t value = sizeof( type );
	};

	// the biggest of everything in Args and First
	template<typename First, typename Second, typename Third, typename... Args>
	struct max_sizeof<First, Second, Third, Args...> {
		using next = typename max_sizeof<Second, Third, Args...>::type;
		using type = typename max_sizeof<First, next>::type;
		static constexpr std::size_t value = sizeof( type );
	};

	template<typename... Types>
	using max_sizeof_t = typename max_sizeof<Types...>::type;

	template<typename... Types>
	inline constexpr std::size_t max_sizeof_v = max_sizeof<Types...>::value;

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Returns true if all values passed are true
	///
	template<typename BoolType,
	         daw::enable_when_t<std::is_convertible_v<BoolType, bool>> = nullptr>
	constexpr bool are_true( BoolType b1 ) noexcept {
		return static_cast<bool>( b1 );
	}

	template<typename BoolType1, typename BoolType2>
	constexpr bool are_true( BoolType1 b1, BoolType2 b2 ) noexcept {
		return are_true( b1 ) and are_true( b2 );
	}

	template<typename BoolType1, typename BoolType2, typename... Booleans>
	constexpr bool are_true( BoolType1 b1, BoolType2 b2,
	                         Booleans... others ) noexcept {
		return are_true( b1, b2 ) and are_true( others... );
	}

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Are all template parameters the same type
	///
	template<typename T, typename... Ts>
	inline constexpr bool are_same_types_v = ( std::is_same_v<T, Ts> and ... );

	template<typename T, typename... Ts>
	using are_same_types = std::bool_constant<are_same_types_v<T, Ts...>>;

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	A sequence of bool values
	///
	template<bool...>
	struct bool_sequence {};

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Integral constant with result of and'ing all bool's supplied
	///
	template<bool... Bools>
	inline constexpr bool bool_and_v = ( Bools and ... );

	template<bool... Bools>
	using bool_and = std::bool_constant<bool_and_v<Bools...>>;

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Integral constant with result of or'ing all bool's supplied
	///
	template<bool... Bools>
	inline constexpr bool bool_or_v = ( Bools or ... );

	template<bool... Bools>
	using bool_or = std::bool_constant<bool_or_v<Bools...>>;
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

	template<typename To, typename... Froms>
	inline constexpr bool can_convert_from_v =
	  true or ( std::is_convertible_v<To, Froms> or ... );

	template<typename To, typename... Froms>
	using can_convert_from = std::bool_constant<can_convert_from_v<To, Froms...>>;

	namespace traits_details {
		template<typename>
		struct type_sink {
			using type = void;
		}; // consumes a type, and makes it `void`
		template<typename T>
		using type_sink_t = typename type_sink<T>::type;
	} // namespace traits_details

	DAW_MAKE_REQ_TRAIT_TYPE( has_type_member_v, T::type );

	DAW_MAKE_REQ_TRAIT_TYPE( has_value_type_member_v, T::value_type );

	DAW_MAKE_REQ_TRAIT_TYPE( has_mapped_type_member_v, T::mapped_type );

	DAW_MAKE_REQ_TRAIT_TYPE( has_iterator_member_v, T::iterator );

	DAW_MAKE_REQ_TRAIT( has_begin_member_v, std::declval<T &>( ).begin( ) );

	DAW_MAKE_REQ_TRAIT( has_end_member_v, std::declval<T &>( ).end( ) );

	DAW_MAKE_REQ_TRAIT( has_substr_member_v,
	                    std::declval<T>( ).substr( std::declval<size_t>( ),
	                                               std::declval<size_t>( ) ) );

	template<typename T>
	inline constexpr bool is_map_like_v =
	  all_true_v<is_container_like_v<T>, has_mapped_type_member_v<T>>;

	template<typename T>
	inline constexpr bool isnt_map_like_v = not is_map_like_v<T>;

	template<typename T>
	using static_not = std::bool_constant<not T::value>;

	template<typename T>
	using static_not_t = typename static_not<T>::type;

	template<typename T>
	inline constexpr bool static_not_v = static_not<T>::value;

	template<typename OutStream, typename T, typename = void>
	inline constexpr bool is_streamable_v = false;

	template<typename OutStream, typename T>
	inline constexpr bool
	  is_streamable_v<OutStream, T,
	                  std::void_t<decltype( std::declval<OutStream &>( )
	                                        << std::declval<T>( ) )>> = true;

	template<template<class> class Base, typename Derived>
	inline constexpr bool is_mixed_from_v =
	  std::is_base_of_v<Base<Derived>, Derived>;

	namespace traits_details {
		namespace operators {
			template<typename L, typename R>
			[[maybe_unused]] auto has_op_eq_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_eq_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs == rhs ), bool>;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_ne_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_ne_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs != rhs ), bool>;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_lt_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_lt_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs < rhs ), bool>;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_le_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_le_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs <= rhs ), bool>;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_gt_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_gt_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs > rhs ), bool>;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_ge_impl( L const &, R const &, long )
			  -> std::false_type;

			template<typename L, typename R>
			[[maybe_unused]] auto has_op_ge_impl( L const &lhs, R const &rhs, int )
			  -> std::is_convertible<decltype( lhs >= rhs ), bool>;
		} // namespace operators
	} // namespace traits_details

	namespace operators {
		template<typename L, typename R = L>
		inline constexpr bool has_op_eq_v =
		  decltype( traits_details::operators::has_op_eq_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		template<typename L, typename R = L>
		inline constexpr bool has_op_ne_v =
		  decltype( traits_details::operators::has_op_ne_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		template<typename L, typename R = L>
		inline constexpr bool has_op_lt_v =
		  decltype( traits_details::operators::has_op_lt_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		template<typename L, typename R = L>
		inline constexpr bool has_op_le_v =
		  decltype( traits_details::operators::has_op_le_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		template<typename L, typename R = L>
		inline constexpr bool has_op_gt_v =
		  decltype( traits_details::operators::has_op_gt_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;

		template<typename L, typename R = L>
		inline constexpr bool has_op_ge_v =
		  decltype( traits_details::operators::has_op_ge_impl(
		    std::declval<L>( ), std::declval<R>( ), 1 ) )::value;
	} // namespace operators

	template<typename T, typename U>
	constexpr inline bool not_self( ) {
		using decayed_t = typename std::decay_t<T>;
		return not std::is_same_v<decayed_t, U> and
		       not std::is_base_of_v<U, decayed_t>;
	}

	template<typename To, typename... From>
	inline constexpr bool are_convertible_to_v =
	  all_true_v<std::is_convertible_v<From, To>...>;

	namespace traits_impl {
		template<typename T, std::size_t N, std::size_t M>
		void is_array_of_array_test( T ( & )[N][M] );
	} // namespace traits_impl

	DAW_MAKE_REQ_TRAIT( is_array_array_v, traits_impl::is_array_of_array_test(
	                                        std::declval<T>( ) ) );

	template<typename T>
	inline constexpr bool is_not_array_array_v = not is_array_array_v<T>;

	template<typename String>
	inline constexpr bool is_string_view_like_v =
	  is_container_like_v<String const> and
	  has_integer_subscript_v<String const> and
	  has_size_memberfn_v<String const> and has_empty_memberfn_v<String const> and
	  is_not_array_array_v<String>;

	template<typename String>
	inline constexpr bool is_string_like_v =
	  is_string_view_like_v<String> and has_append_operator_v<String> and
	  has_append_memberfn_v<String> and is_container_like_v<String> and
	  has_integer_subscript_v<String>;

	template<typename Container, std::size_t ExpectedSize>
	inline constexpr bool is_value_size_equal_v =
	  sizeof( *std::cbegin( std::declval<Container>( ) ) ) == ExpectedSize;

	namespace traits_details {
		template<typename...>
		struct are_unique;

		template<typename T1>
		struct are_unique<T1> : std::true_type {};

		template<typename T1, typename T2>
		struct are_unique<T1, T2> : std::bool_constant<not std::is_same_v<T1, T2>> {
		};

		template<typename T1, typename T2, typename... Ts>
		struct are_unique<T1, T2, Ts...>
		  : std::bool_constant<are_unique<T1, T2>::value and
		                       are_unique<T1, Ts...>::value and
		                       are_unique<T2, Ts...>::value> {};
	} // namespace traits_details

	template<typename... Ts>
	using are_unique_t = traits_details::are_unique<Ts...>;
	template<typename... Ts>
	inline constexpr bool are_unique_v = are_unique_t<Ts...>::value;

	namespace traits_details {
		template<typename...>
		struct isnt_cv_ref;

		template<>
		struct isnt_cv_ref<> : std::true_type {};

		template<typename T, typename... Ts>
		struct isnt_cv_ref<T, Ts...>
		  : std::bool_constant<(
		      not std::disjunction_v<daw::traits::is_const<T>, std::is_reference<T>,
		                             std::is_volatile<T>> and
		      std::is_same_v<std::true_type, typename isnt_cv_ref<Ts...>::type> )> {
		};
	} // namespace traits_details

	template<typename... Ts>
	inline constexpr bool isnt_cv_ref_v =
	  traits_details::isnt_cv_ref<Ts...>::value;

	template<typename T>
	using deref_t = decltype( *std::declval<T>( ) );

	template<typename Function, typename... Args>
	using result_of_t =
	  decltype( std::declval<Function>( )( std::declval<Args>( )... ) );

	template<typename Function, typename... Args>
	using invoke_result_t =
	  decltype( std::declval<Function>( )( std::declval<Args>( )... ) );

	namespace traits_details {
		template<typename>
		inline constexpr bool is_tuple_impl_v = false;

		template<typename... Ts>
		inline constexpr bool is_tuple_impl_v<std::tuple<Ts...>> = true;

	} // namespace traits_details
	template<typename T>
	inline constexpr bool is_tuple_v =
	  traits_details::is_tuple_impl_v<daw::remove_cvref_t<T>>;

	template<typename...>
	struct is_first_type;

	template<typename T>
	struct is_first_type<T> : std::false_type {};

	template<typename T, typename Arg, typename... Args>
	struct is_first_type<T, Arg, Args...> : std::is_same<T, std::decay_t<Arg>> {};

	// Tests if type T is the same as the first argument in Args or if args is
	// empty it is false
	template<typename T, typename... Args>
	inline constexpr bool is_first_type_v = is_first_type<T, Args...>::value;

	template<typename T, typename... Args>
	inline constexpr bool is_init_list_constructible_v = all_true_v<
	  are_same_types_v<Args...>,
	  std::is_constructible_v<T, std::initializer_list<first_type<Args...>>>>;

	template<typename T>
	inline constexpr bool is_character_v = is_one_of_v<T, char, wchar_t>;

#if defined( DAW_HAS_CLANG )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#endif
	template<typename... Args>
	DAW_CPP20_REQUIRES( sizeof...( Args ) > 0 )
	using last_type_t = typename decltype( ( ( identity<Args>{ } ), ... ) )::type;
#if defined( DAW_HAS_CLANG )
#pragma clang diagnostic pop
#endif

	namespace traits_details::pack_index_of {
		template<int Index, typename A, typename B, typename... C>
		[[maybe_unused]] constexpr int pack_index_of_calc( ) noexcept {
			if constexpr( std::is_same_v<A, B> ) {
				return Index;
			} else if constexpr( sizeof...( C ) > 0 ) {
				return pack_index_of_calc<Index + 1, A, C...>( );
			} else {
				return -1;
			}
		}
	} // namespace traits_details::pack_index_of

	template<typename A, typename B, typename... C>
	struct pack_index_of
	  : std::integral_constant<
	      int,
	      traits_details::pack_index_of::pack_index_of_calc<0, A, B, C...>( )> {};

	template<typename T, typename... Args>
	struct pack_list_front {
		using front = T;
		using back = pack_list<Args...>;
	};

	template<typename... Args>
	using pop_list_front = pack_list_front<Args...>;

	template<typename T, typename... Args>
	using push_list_back = pack_list<Args..., T>;

	template<typename T, typename... Args>
	using push_list_front = pack_list<T, Args...>;

	template<typename A, typename B>
	struct pack_index_of<A, B>
	  : std::integral_constant<int, ( std::is_same<A, B>{ } - 1 )> {};

	template<typename T, typename... Pack>
	struct pack_index_of<T, pack_list<Pack...>> : pack_index_of<T, Pack...> {};

	template<typename T, typename... Pack>
	inline constexpr auto pack_index_of_v = pack_index_of<T, Pack...>::value;

	template<typename T, typename... Pack>
	inline constexpr bool pack_exists_v = pack_index_of_v<T, Pack...> >= 0;

	template<template<typename...> typename, typename>
	inline constexpr bool is_instance_of_v = false;

	template<template<typename...> typename Template, typename... Args>
	inline constexpr bool is_instance_of_v<Template, Template<Args...>> = true;

	template<template<typename...> typename Template, typename Type>
	using is_instance_of = std::bool_constant<is_instance_of_v<Template, Type>>;

	template<typename F, bool IsNoExcept, typename R, typename... Args>
	struct lifted_t {
		F fp;

		constexpr R operator( )( Args... args ) const noexcept( IsNoExcept ) {
			return fp( DAW_FWD( args )... );
		}
	};

	template<typename>
	struct func_traits;

	template<typename R, typename... Args>
	struct func_traits<R( Args... )> {
		using return_type = R;
		static constexpr std::size_t arity = sizeof...( Args );

		static constexpr bool is_noexcept = false;
		template<size_t N>
		struct argument {
			static_assert( N < arity, "Attempt to access argument out of range." );
			using type = nth_element<N, Args...>;
		};

		template<typename Func>
		static constexpr lifted_t<Func, is_noexcept, R, Args...>
		lift( Func &&f ) noexcept {
			return { DAW_FWD( f ) };
		}
	};

	template<typename R, typename... Args>
	struct func_traits<R( Args... ) noexcept> {
		using return_type = R;
		static constexpr std::size_t arity = sizeof...( Args );

		static constexpr bool is_noexcept = true;
		template<size_t N>
		struct argument {
			static_assert( N < arity, "Attempt to access argument out of range." );
			using type = nth_element<N, Args...>;
		};

		template<typename Func>
		static constexpr lifted_t<Func, is_noexcept, R, Args...>
		lift( Func &&f ) noexcept {
			return { DAW_FWD( f ) };
		}
	};

	template<class R, class... Args>
	struct func_traits<R ( * )( Args... )> : public func_traits<R( Args... )> {};

	template<class R, class... Args>
	struct func_traits<R ( & )( Args... )> : public func_traits<R( Args... )> {};

	template<class R, class... Args>
	struct func_traits<R ( * )( Args... ) noexcept>
	  : public func_traits<R( Args... ) noexcept> {};

	template<class R, class... Args>
	struct func_traits<R ( & )( Args... ) noexcept>
	  : public func_traits<R( Args... ) noexcept> {};

	template<typename F, std::size_t N>
	using func_arg_t = typename func_traits<F>::template argument<N>::type;

	template<typename F>
	using func_result_t = typename func_traits<F>::return_type;

	template<typename F>
	[[nodiscard]] constexpr decltype( auto ) lift_func( F &&f ) noexcept {
		if constexpr( std::is_function_v<daw::remove_cvref_t<F>> ) {
			using func_t = func_traits<F>;
			static_assert( func_t::arity == 1,
			               "Only single argument overloads are supported" );
			return func_t::lift( DAW_FWD( f ) );
		} else {
			return DAW_FWD( f );
		}
	}

	template<typename T>
	using fn_t = std::add_pointer_t<T>;

	namespace traits_details {
		template<typename T, typename... Ts>
		struct make_something {
			using type = T;
		};
	} // namespace traits_details

	/***
	 * Similar to void T, will always be T
	 * @tparam T Type to always be
	 * @tparam Ts Types to SFINAE off of
	 */
	template<typename T, typename... Ts>
	using something_t = typename traits_details::make_something<T, Ts...>::type;

	struct no_else {};

	template<bool Bool, typename OnTrue, typename OnFalse = no_else>
	constexpr decltype( auto ) cxif( OnTrue ot = OnTrue{ },
	                                 OnFalse of = OnFalse{ } ) {
		if constexpr( Bool ) {
			return ot( );
		} else {
			return of( );
		}
	}

	namespace traits_details {
		template<typename T, typename = void>
		inline constexpr bool is_list_constructible_impl_v = false;

		template<typename T, typename... Args>
		inline constexpr bool is_list_constructible_impl_v<
		  daw::pack_list<T, Args...>,
		  std::void_t<decltype( T{ std::declval<Args>( )... } )>> = true;
	} // namespace traits_details

	template<typename T, typename... Args>
	inline constexpr bool is_list_constructible_v =
	  traits_details::is_list_constructible_impl_v<daw::pack_list<T, Args...>>;

	template<typename T, typename... Args>
	using is_list_constructible =
	  std::bool_constant<is_list_constructible_v<T, Args...>>;

	namespace traits_details {
		template<bool IsListConstructible, typename T, typename... Args>
		struct is_nothrow_list_constructible;

		template<typename T, typename... Args>
		struct is_nothrow_list_constructible<true, T, Args...> {
			using type =
			  std::bool_constant<noexcept( T{ std::declval<Args>( )... } )>;
		};

		template<typename T, typename... Args>
		struct is_nothrow_list_constructible<false, T, Args...> {
			using type = std::false_type;
		};

	} // namespace traits_details
	template<typename T, typename... Args>
	struct is_nothrow_list_constructible
	  : traits_details::is_nothrow_list_constructible<
	      is_list_constructible_v<T, Args...>, T, Args...>::type {};

	template<typename T, typename... Args>
	inline constexpr bool is_nothrow_list_constructible_v =
	  is_nothrow_list_constructible<T, Args...>::value;

	///
	// Query the parts of a classes template parameters, if any
	namespace class_parts_details {
		template<std::size_t Idx, typename T, typename Tp>
		constexpr bool update_if_same( std::size_t &result, std::size_t NotFound ) {
			if constexpr( std::is_same_v<T, pack_element_t<Idx, Tp>> ) {
				if( result == NotFound ) {
					result = Idx;
				}
			}
			return result == NotFound;
		}

		template<std::size_t Idx, typename T, typename Tp>
		constexpr bool update_if_different( std::size_t &result,
		                                    std::size_t NotFound ) {
			if constexpr( not std::is_same_v<T, pack_element_t<Idx, Tp>> ) {
				if( result == NotFound ) {
					result = Idx;
				}
			}
			return result == NotFound;
		}

		template<std::size_t StartIdx, typename T, typename Tp, std::size_t... Idx>
		constexpr void find_template_parameter( std::size_t &result,
		                                        std::index_sequence<Idx...>,
		                                        std::size_t NotFound ) {
			(void)( update_if_same<StartIdx + Idx, T, Tp>( result, NotFound ) &&
			        ... );
		}

		template<std::size_t StartIdx, typename T, typename... Args>
		constexpr std::size_t find_template_parameter( std::size_t NotFound ) {
			static_assert( StartIdx < sizeof...( Args ) );
			std::size_t result = NotFound;
			find_template_parameter<StartIdx, T, pack_list<Args...>>(
			  result, std::make_index_sequence<sizeof...( Args ) - StartIdx>{ },
			  NotFound );
			return result;
		}

		template<std::size_t StartIdx, typename Lhs, typename Rhs,
		         std::size_t... Idx>
		constexpr std::size_t find_first_mismatch( Lhs *, Rhs *,
		                                           std::index_sequence<Idx...>,
		                                           std::size_t NotFound ) {
			std::size_t result = NotFound;
			(void)( update_if_different<StartIdx + Idx,
			                            pack_element_t<StartIdx + Idx, Lhs>, Rhs>(
			          result, NotFound ) &&
			        ... );
			return result;
		}

		template<std::size_t StartIdx, typename Lhs, typename Rhs>
		constexpr std::size_t find_first_mismatch( Lhs *, Rhs * ) {
			using TpL = typename Lhs::class_template_parameters;
			using TpR = typename Rhs::class_template_parameters;
			constexpr std::size_t Max = []( auto const &l, auto const &r ) {
				return r < l ? r : l;
			}( pack_size_v<TpL>, pack_size_v<TpL> );
			return find_first_mismatch<StartIdx>(
			  static_cast<TpL *>( nullptr ), static_cast<TpR *>( nullptr ),
			  std::make_index_sequence<Max - StartIdx>{ }, Max );
		}
	} // namespace class_parts_details

	template<typename C>
	struct class_parts {
		using class_type = C;
		static constexpr bool is_templated_class = false;

		template<typename>
		using has_class_template_parameter = std::false_type;

		using NotFound =
		  std::integral_constant<std::size_t, static_cast<std::size_t>( -1 )>;

		template<typename T>
		using find_class_template_parameter = NotFound;
	};

	template<template<typename...> class C, typename... Args>
	struct class_parts<C<Args...>> {
		using class_type = C<Args...>;
		using class_template_parameters = std::tuple<Args...>;
		static constexpr bool is_templated_class = true;

		template<std::size_t Idx>
		using nth_class_template_parameter = nth_element<Idx, Args...>;

		template<typename T>
		using has_class_template_parameter =
		  std::bool_constant<std::disjunction_v<std::is_same<T, Args>...>>;

		using NotFound =
		  std::integral_constant<std::size_t, static_cast<std::size_t>( -1 )>;

		template<typename T, std::size_t StartIdx = 0>
		using find_class_template_parameter =
		  std::integral_constant<std::size_t,
		                         class_parts_details::find_template_parameter<
		                           StartIdx, T, Args...>( NotFound{ } )>;
		template<typename OtherClass, std::size_t StartIdx = 0>
		using find_first_mismatched_class_template_parameter =
		  std::integral_constant<std::size_t,
		                         class_parts_details::find_first_mismatch<StartIdx>(
		                           static_cast<class_parts *>( nullptr ),
		                           static_cast<OtherClass *>( nullptr ) )>;
	};

	template<typename T>
	inline constexpr bool is_templated_class = class_parts<T>::is_templated_class;

	template<std::size_t Idx, typename T>
	using nth_class_template_parameter =
	  typename class_parts<T>::template nth_class_template_parameter<Idx>;

	template<typename T>
	using class_template_parameters =
	  typename class_parts<T>::class_template_parameters;

	template<typename T, typename Class>
	using has_class_template_parameter =
	  typename class_parts<Class>::template has_class_template_parameter<T>;

	template<typename T, typename Class>
	inline constexpr bool has_class_template_parameter_v =
	  has_class_template_parameter<T, Class>::value;

	template<typename T, typename Class, std::size_t StartIdx = 0>
	using find_class_template_parameter = typename class_parts<
	  Class>::template find_class_template_parameter<T, StartIdx>;

	template<typename T, typename Class, std::size_t StartIdx = 0>
	inline constexpr std::size_t find_class_template_parameter_v =
	  find_class_template_parameter<T, Class, StartIdx>::value;

	template<typename ClassA, typename ClassB, std::size_t StartIdx = 0>
	using find_first_mismatched_class_template_parameter =
	  typename class_parts<ClassA>::
	    template find_first_mismatched_class_template_parameter<
	      class_parts<ClassB>, StartIdx>;

	template<typename ClassA, typename ClassB, std::size_t StartIdx = 0>
	inline constexpr std::size_t
	  find_first_mismatched_class_template_parameter_v =
	    find_first_mismatched_class_template_parameter<ClassA, ClassB,
	                                                   StartIdx>::value;

	template<typename, typename>
	struct not_same : std::true_type {};

	template<typename T>
	struct not_same<T, T> : std::false_type {};

	template<typename T, typename U>
	inline constexpr bool not_same_v = not_same<T, U>::value;

	template<typename To, typename From>
	using copy_ref_t = conditional_t<
	  /*if*/ std::is_lvalue_reference_v<From>,
	  /*then*/ std::add_lvalue_reference_t<std::remove_reference_t<To>>,
	  /*else if*/
	  conditional_t<
	    /*if*/ daw::traits::is_rvalue_reference_v<From>,
	    /*then*/ std::add_rvalue_reference_t<std::remove_reference_t<To>>,
	    /*else*/ std::remove_reference_t<To>
	    /**/>
	  /**/>;

	template<typename To, typename From>
	using copy_const_t = conditional_t<
	  /*if*/ daw::traits::is_const_v<std::remove_reference_t<From>>,
	  /*then*/ copy_ref_t<To, std::add_const_t<std::remove_reference_t<To>>>,
	  /*else*/ copy_ref_t<To, std::remove_const_t<std::remove_reference_t<To>>>
	  /**/>;

	template<typename To, typename From>
	using copy_constref_t = copy_ref_t<From, copy_const_t<To, From>>;

	template<typename T>
	struct member_type_of;

	template<typename T, typename M>
	struct member_type_of<M T::*> {
		using type = M;
	};

	template<typename T>
	using member_type_of_t = typename member_type_of<T>::type;

	template<auto Value, auto... Values>
	inline constexpr bool equal_to_any_of_v = ( ( Value == Values ) or ... );
} // namespace daw::traits

namespace daw {
	template<std::size_t N>
	using index_constant = std::integral_constant<std::size_t, N>;

	template<std::size_t N>
	inline constexpr index_constant<N> index_constant_v = index_constant<N>{ };

	template<std::size_t Index, typename... Cases>
	using switch_t = pack_element_t<Index, pack_list<Cases...>>;

	template<typename>
	struct template_param {};

	template<typename...>
	struct template_params {};

	template<typename... Ts>
	inline constexpr template_params<Ts...> template_args =
	  template_params<Ts...>{ };

	template<typename T>
	inline constexpr template_param<T> template_arg = template_param<T>{ };

	template<auto... vals>
	struct template_vals_t {};

	template<auto... vals>
	inline constexpr template_vals_t<vals...> template_vals =
	  template_vals_t<vals...>{ };

	template<typename... Ts>
	struct expander {
		explicit constexpr expander( Ts const &... ) noexcept {}
	};
	template<typename... Ts>
	expander( Ts... ) -> expander<Ts...>; // no warnings about intent to use CTAD

	template<template<typename...> typename Primary, typename T>
	inline constexpr bool is_specialization_of_v = false;

	template<template<typename...> typename Primary, typename... Args>
	inline constexpr bool is_specialization_of_v<Primary, Primary<Args...>> =
	  true;

	template<template<typename...> typename Primary, typename T>
	using is_specialization_of =
	  std::bool_constant<is_specialization_of_v<Primary, T>>;

#if defined( DAW_CPP20_CONCEPTS )
	template<typename T, template<typename...> typename Template>
	concept specialization_of = is_specialization_of_v<Template, T>;
#endif

	template<typename T, bool>
	struct dependent_type : T {};

	namespace is_complete_details {
		template<typename T>
		auto is_complete_test( T * )
		  -> decltype( (void)sizeof( T ), std::true_type{ } );
		auto is_complete_test( ... ) -> std::false_type;
	} // namespace is_complete_details

	template<typename T>
	inline constexpr bool is_complete_type_v =
	  decltype( is_complete_details::is_complete_test(
	    std::declval<T *>( ) ) ){ };

	template<typename T>
	using is_complete_type = std::bool_constant<is_complete_type_v<T>>;

	template<typename T>
	struct rvalue_to_value {
		using type = T;
	};

	template<typename T>
	struct rvalue_to_value<T &> {
		using type = T &;
	};

	template<typename T>
	struct rvalue_to_value<T &&> {
		using type = std::remove_cv_t<T>;
	};

	template<typename T>
	using rvalue_to_value_t = typename rvalue_to_value<T>::type;

	template<typename...>
	inline constexpr bool deduced_false_v = false;

	template<typename T>
	using fp_t = T *;

	template<typename T>
	inline constexpr std::size_t const bsizeof =
	  bit_count_v<daw::remove_cvref_t<T>>;

	template<typename...>
	using void_t = void;

#if defined( DAW_HAS_CPP_CONCEPTS )
	template<typename Tp>
	inline constexpr bool is_tuple_like_v = requires {
		std::tuple_size_v<daw::remove_cvref_t<Tp>>;
		std::get<0>( std::declval<Tp>( ) );
		typename std::tuple_element_t<0, daw::remove_cvref_t<Tp>>;
	};
#else
	template<typename Tp, typename = void>
	inline constexpr bool is_tuple_like_v = false;

	template<typename Tp>
	inline constexpr bool is_tuple_like_v<
	  Tp, void_t<decltype( (void)(std::tuple_size_v<daw::remove_cvref_t<Tp>>),
	                       (void)( std::get<0>( std::declval<Tp>( ) ) ) ),
	             typename std::tuple_element_t<0, daw::remove_cvref_t<Tp>>>> =
	  true;
#endif

	template<auto, typename T>
	using ignore_one_t = T;
} // namespace daw

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
#include "daw_enable_if.h"
#include "daw_move.h"
#include "impl/daw_conditional.h"

#include <cstddef>
#include <cstring>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template<bool B>
	using bool_constant = std::bool_constant<B>;

	template<typename B>
	struct negation : bool_constant<not bool( B::value )> {};

	template<typename...>
	struct conjunction : std::true_type {};

	template<typename B1>
	struct conjunction<B1> : B1 {};

	template<typename B1, typename... Bn>
	struct conjunction<B1, Bn...>
	  : conditional_t<static_cast<bool>( B1::value ), conjunction<Bn...>,
	                       B1> {};

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

			[[maybe_unused]] explicit constexpr not_fn_t( Function &&func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : m_function{ DAW_MOVE( func ) } {}

			[[maybe_unused]] explicit constexpr not_fn_t(
			  Function const
			    &func ) noexcept( std::is_nothrow_copy_constructible_v<Function> )
			  : m_function{ func } {}

			template<typename... Args>
			[[nodiscard, maybe_unused]] constexpr decltype( auto )
			operator( )( Args &&...args ) noexcept(
			  std::is_nothrow_invocable_v<Function, Args...> ) {
				return not m_function( DAW_FWD( args )... );
			}

			template<typename... Args>
			[[nodiscard, maybe_unused]] constexpr decltype( auto )
			operator( )( Args &&...args ) const
			  noexcept( std::is_nothrow_invocable_v<Function, Args...> ) {
				return not m_function( DAW_FWD( args )... );
			}
		};
	} // namespace cpp_17_details

	template<typename Function>
	[[nodiscard, maybe_unused]] constexpr auto not_fn( Function &&func ) {
		using func_t = remove_cvref_t<Function>;
		return cpp_17_details::not_fn_t<func_t>( DAW_FWD( func ) );
	}

	template<typename Function>
	[[nodiscard, maybe_unused]] constexpr auto not_fn( ) {
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
	[[nodiscard, maybe_unused]] constexpr std::add_const_t<T> &
	as_const( T &t ) noexcept {
		return t;
	}

	template<template<class...> class, class...>
	struct nonesuch {
		nonesuch( ) = delete; // prevent pre C++20 aggregate construction
		~nonesuch( ) = delete;
		nonesuch( nonesuch const & ) = delete;
		nonesuch operator=( nonesuch const & ) = delete;
		nonesuch( nonesuch && ) = delete;
		nonesuch &operator=( nonesuch && ) = delete;
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
	namespace cpp_17_details {
		template<class Default, class AlwaysVoid, template<class...> class Op,
		         class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template<class Default, class AlwaysVoid, template<class...> class Op,
		         class... Args>
		inline constexpr bool detector_v = false;

		template<class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

		template<class Default, template<class...> class Op, class... Args>
		inline constexpr bool
		  detector_v<Default, std::void_t<Op<Args...>>, Op, Args...> = true;
	} // namespace cpp_17_details

	template<template<class...> class Op, class... Args>
	using is_detected =
	  typename cpp_17_details::detector<nonesuch<Op, Args...>, void, Op,
	                                    Args...>::value_t;

	template<template<class...> class Op, class... Args>
	using detected_t = typename cpp_17_details::detector<nonesuch<Op, Args...>,
	                                                     void, Op, Args...>::type;

#ifndef DAW_HAS_CONCEPTS
	template<template<class...> class Op, class... Args>
	inline constexpr bool is_detected_v =
	  cpp_17_details::detector_v<nonesuch<Op, Args...>, void, Op, Args...>;

#endif
	template<class Default, template<class...> class Op, class... Args>
	using detected_or = cpp_17_details::detector<Default, void, Op, Args...>;

	template<class Default, template<class...> class Op, class... Args>
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;

	template<class Expected, template<class...> class Op, class... Args>
	inline constexpr bool is_detected_exact_v =
	  std::is_same_v<Expected, detected_t<Op, Args...>>;

	template<class Expected, template<class...> class Op, class... Args>
	using is_detected_exact =
	  std::bool_constant<is_detected_exact_v<Expected, Op, Args...>>;

	template<class To, template<class...> class Op, class... Args>
	using is_detected_convertible =
	  std::is_convertible<detected_t<Op, Args...>, To>;

	template<class To, template<class...> class Op, class... Args>
	inline constexpr bool is_detected_convertible_v =
	  is_detected_convertible<To, Op, Args...>::value;

	template<typename Container>
	[[nodiscard, maybe_unused]] constexpr auto
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

	namespace cpp_17_details {
		template<typename Base, typename T, typename Derived, typename... Args>
		[[nodiscard, maybe_unused]] auto
		INVOKE( T Base::*pmf, Derived &&ref, Args &&...args ) noexcept(
		  noexcept( ( DAW_FWD( ref ).*pmf )( DAW_FWD( args )... ) ) )
		  -> std::enable_if_t<
		    std::conjunction_v<std::is_function<T>,
		                       std::is_base_of<Base, std::decay_t<Derived>>>,
		    decltype( ( DAW_FWD( ref ).*pmf )( DAW_FWD( args )... ) )> {
			return ( DAW_FWD( ref ).*pmf )( DAW_FWD( args )... );
		}

		template<typename Base, typename T, typename RefWrap, typename... Args>
		[[nodiscard, maybe_unused]] constexpr auto
		INVOKE( T Base::*pmf, RefWrap &&ref, Args &&...args ) noexcept(
		  noexcept( ( ref.get( ).*pmf )( DAW_FWD( args )... ) ) )
		  -> std::enable_if_t<
		    std::conjunction_v<std::is_function<T>,
		                       is_reference_wrapper<std::decay_t<RefWrap>>>,
		    decltype( ( ref.get( ).*pmf )( DAW_FWD( args )... ) )> {

			return ( ref.get( ).*pmf )( DAW_FWD( args )... );
		}

		template<typename Base, typename T, typename Pointer, typename... Args>
		[[nodiscard, maybe_unused]] constexpr auto
		INVOKE( T Base::*pmf, Pointer &&ptr, Args &&...args ) noexcept(
		  noexcept( ( ( *DAW_FWD( ptr ) ).*pmf )( DAW_FWD( args )... ) ) )
		  -> std::enable_if_t<
		    std::conjunction_v<
		      std::is_function<T>,
		      not_trait<is_reference_wrapper<std::decay_t<Pointer>>>,
		      not_trait<std::is_base_of<Base, std::decay_t<Pointer>>>>,
		    decltype( ( ( *DAW_FWD( ptr ) ).*pmf )( DAW_FWD( args )... ) )> {

			return ( ( *DAW_FWD( ptr ) ).*pmf )( DAW_FWD( args )... );
		}

		template<typename Base, typename T, typename Derived>
		[[nodiscard, maybe_unused]] constexpr auto
		INVOKE( T Base::*pmd,
		        Derived &&ref ) noexcept( noexcept( DAW_FWD( ref ).*pmd ) )
		  -> std::enable_if_t<
		    std::conjunction_v<not_trait<std::is_function<T>>,
		                       std::is_base_of<Base, std::decay_t<Derived>>>,
		    decltype( DAW_FWD( ref ).*pmd )> {

			return DAW_FWD( ref ).*pmd;
		}

		template<typename Base, typename T, typename RefWrap>
		[[nodiscard, maybe_unused]] constexpr auto
		INVOKE( T Base::*pmd,
		        RefWrap &&ref ) noexcept( noexcept( ref.get( ).*pmd ) )
		  -> std::enable_if_t<
		    std::conjunction_v<not_trait<std::is_function<T>>,
		                       is_reference_wrapper<std::decay_t<RefWrap>>>,
		    decltype( ref.get( ).*pmd )> {
			return ref.get( ).*pmd;
		}

		template<typename Base, typename T, typename Pointer>
		[[nodiscard, maybe_unused]] constexpr auto
		INVOKE( T Base::*pmd,
		        Pointer &&ptr ) noexcept( noexcept( ( *DAW_FWD( ptr ) ).*pmd ) )
		  -> std::enable_if_t<
		    std::conjunction_v<
		      not_trait<std::is_function<T>>,
		      not_trait<is_reference_wrapper<std::decay_t<Pointer>>>,
		      not_trait<std::is_base_of<Base, std::decay_t<Pointer>>>>,
		    decltype( ( *DAW_FWD( ptr ) ).*pmd )> {
			return ( *DAW_FWD( ptr ) ).*pmd;
		}

		template<typename T, typename... Args>
		using detect_call_operator =
		  decltype( std::declval<T>( ).operator( )( std::declval<Args>( )... ) );

		template<typename F, typename... Args>
		[[nodiscard, maybe_unused]] constexpr decltype( auto ) INVOKE(
		  F &&f,
		  Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

			return DAW_FWD( f )( DAW_FWD( args )... );
		}
	} // namespace cpp_17_details

#if not defined( _MSC_VER ) or defined( __clang__ )
	template<typename F, typename... Args,
	         daw::enable_when_t<std::conjunction_v<
	           not_trait<is_reference_wrapper<Args>>...>> = nullptr>
	[[nodiscard]] constexpr decltype( auto )
	invoke( F &&f,
	        Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

		return cpp_17_details::INVOKE( DAW_FWD2( F, f ),
		                               DAW_FWD2( Args, args )... );
	}

	template<typename F, typename... Args>
	[[nodiscard]] constexpr decltype( auto )
	invoke( F &&f, std::reference_wrapper<Args>... args ) noexcept(
	  std::is_nothrow_invocable_v<F, Args...> ) {

		return cpp_17_details::INVOKE( DAW_FWD2( F, f ),
		                               DAW_FWD2( Args, args )... );
	}
#else
	template<typename F, typename... Args>
	constexpr decltype( auto )
	invoke( F &&f,
	        Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

		return DAW_FWD2( F, f )( DAW_FWD2( Args, args )... );
	}
#endif

	namespace cpp_17_details {
		template<typename F, typename Tuple, std::size_t... I>
		[[nodiscard, maybe_unused]] constexpr decltype( auto )
		apply_details( F &&f, Tuple &&t, std::index_sequence<I...> ) {
			return daw::invoke( DAW_FWD( f ), std::get<I>( DAW_FWD( t ) )... );
		}

		template<typename>
		inline constexpr bool is_tuple_v = false;

		template<typename... Args>
		inline constexpr bool is_tuple_v<std::tuple<Args...>> = true;

		template<typename T>
		using is_tuple = std::bool_constant<is_tuple_v<T>>;

		template<typename F, typename Tuple, std::size_t... Is>
		[[nodiscard]] inline constexpr decltype( auto )
		apply_impl2( F &&f, Tuple &&t, std::index_sequence<Is...> ) {
			return DAW_FWD( f )( std::get<Is>( DAW_FWD( t ) )... );
		}
	} // namespace cpp_17_details

	template<bool use_invoke = true, typename F, typename Tuple>
	[[nodiscard, maybe_unused]] inline constexpr decltype( auto )
	apply( F &&f, Tuple &&t ) {
		static_assert( cpp_17_details::is_tuple_v<Tuple>,
		               "Attempt to call apply with invalid arguments.  The "
		               "arguments must be a std::tuple" );
		if constexpr( use_invoke ) {
			if constexpr( std::tuple_size_v<Tuple> == 0 ) {
				return daw::invoke( DAW_FWD( f ) );
			} else {
				return cpp_17_details::apply_details(
				  DAW_FWD( f ), DAW_FWD( t ),
				  std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{ } );
			}
		} else {
			constexpr std::size_t tp_size = std::tuple_size_v<Tuple>;
			if constexpr( tp_size == 0 ) {
				return DAW_FWD( f )( );
			} else {
				return cpp_17_details::apply_impl2(
				  DAW_FWD( f ), DAW_FWD( t ), std::make_index_sequence<tp_size>{ } );
			}
		}
	}

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
			[[nodiscard, maybe_unused]] constexpr std::common_type_t<T, U>( min )(
			  T const &lhs, U const &rhs ) noexcept {
				if( lhs <= rhs ) {
					return lhs;
				}
				return rhs;
			}

			template<typename T, typename U, typename V>
			[[nodiscard, maybe_unused]] constexpr T
			clamp( T val, U const &min_val, V const &max_val ) noexcept {
				if( val < min_val ) {
					val = min_val;
				} else if( val > max_val ) {
					val = max_val;
				}
				return val;
			}
		} // namespace math

		template<typename InputIterator>
		[[nodiscard, maybe_unused]] constexpr ptrdiff_t
		distance_impl( InputIterator first, InputIterator last,
		               std::input_iterator_tag ) noexcept( noexcept( ++first ) ) {

			ptrdiff_t count = 0;
			while( first != last ) {
				++count;
				++first;
			}
			return count;
		}

		template<typename Iterator1, typename Iterator2>
		[[nodiscard, maybe_unused]] constexpr ptrdiff_t distance_impl(
		  Iterator1 first, Iterator2 last,
		  std::random_access_iterator_tag ) noexcept( noexcept( last - first ) ) {

			return last - first;
		}

		template<typename Iterator, typename Distance>
		[[maybe_unused]] constexpr void
		advance( Iterator &first, Distance n,
		         std::input_iterator_tag ) noexcept( noexcept( ++first ) ) {

			while( n-- > 0 ) {
				++first;
			}
		}

		template<typename Iterator, typename Distance>
		[[maybe_unused]] constexpr void
		advance( Iterator &first, Distance n,
		         std::output_iterator_tag ) noexcept( noexcept( ++first ) ) {

			while( n-- > 0 ) {
				++first;
			}
		}

		template<typename Iterator, typename Distance>
		[[maybe_unused]] constexpr void advance(
		  Iterator &first, Distance n,
		  std::bidirectional_iterator_tag ) noexcept( noexcept( ++first )
		                                                and noexcept( --first ) ) {

			if( n >= 0 ) {
				while( n-- > 0 ) {
					++first;
				}
			} else {
				while( ++n < 0 ) {
					--first;
				}
			}
		}

		template<typename Iterator, typename Distance>
		[[maybe_unused]] constexpr void
		advance( Iterator &first, Distance n,
		         std::random_access_iterator_tag ) noexcept( noexcept( first +=
		                                                               static_cast<
		                                                                 ptrdiff_t>(
		                                                                 n ) ) ) {
			first += static_cast<ptrdiff_t>( n );
		}
	} // namespace cpp_17_details

	/// @brief Calculate distance between iterators
	/// @tparam Iterator type of Iterator to compare
	/// @param first first iterator, must be <= second if Iterators are not
	/// RandomAccess
	/// @param second second iterator, must be >= first if Iterators are not
	/// RandomAccess
	/// @return	a ptrdiff_t of how many steps apart iterators are.  If Iterators
	/// are RandomAccess it may be <0, otherwise always greater
	template<typename Iterator>
	[[nodiscard, maybe_unused]] constexpr ptrdiff_t
	distance( Iterator first, Iterator second ) noexcept(
	  noexcept( cpp_17_details::distance_impl(
	    first, second,
	    typename std::iterator_traits<Iterator>::iterator_category{ } ) ) ) {
		return cpp_17_details::distance_impl(
		  first, second,
		  typename std::iterator_traits<Iterator>::iterator_category{ } );
	}

	/// @brief Advance iterator n steps
	/// @tparam Iterator Type of iterator to advance
	/// @tparam Distance A type convertible to an integral type
	/// @param it iterator to advance
	/// @param n how far to move iterator
	template<typename Iterator, typename Distance>
	[[maybe_unused]] constexpr void advance( Iterator &it, Distance n ) {
		cpp_17_details::advance(
		  it, static_cast<ptrdiff_t>( n ),
		  typename std::iterator_traits<Iterator>::iterator_category{ } );
	}

	/// @brief Move iterator forward n steps, if n < 0 it is only defined for
	/// types that are Bidirectional
	/// @tparam Iterator Type of iterator to move forward
	/// @param it Iterator to advance
	/// @param n how far to move forward
	/// @return The resulting iterator of advancing it n steps
	template<typename Iterator>
	[[nodiscard, maybe_unused]] constexpr Iterator
	next( Iterator it, ptrdiff_t n = 1 ) noexcept {

		cpp_17_details::advance(
		  it, n, typename std::iterator_traits<Iterator>::iterator_category{ } );
		return DAW_MOVE( it );
	}

	/// @brief Move iterator backward n steps, if n > 0, only defined for types
	/// that are Bidirectional
	/// @tparam Iterator Type of iterator to move backward
	/// @param it Iterator to advance
	/// @param n how far to move backward
	/// @return The resulting iterator of advancing it n steps
	template<typename Iterator>
	[[nodiscard, maybe_unused]] constexpr Iterator
	prev( Iterator it, ptrdiff_t n = 1 ) noexcept {

		cpp_17_details::advance(
		  it, -n, typename std::iterator_traits<Iterator>::iterator_category{ } );
		return DAW_MOVE( it );
	}

	template<typename To, typename From>
	[[nodiscard, maybe_unused]] To bit_cast( From const *const from ) noexcept(
	  std::is_nothrow_constructible_v<To> ) {

		return DAW_BIT_CAST( To, *from );
	}

	template<typename T, typename Iterator, typename Function>
	void bit_cast_for_each(
	  Iterator first, Iterator last,
	  Function &&func ) noexcept( noexcept( func( std::declval<T>( ) ) ) ) {

		while( first != last ) {
			daw::invoke( func, DAW_BIT_CAST( T, *first ) );
			++first;
		}
	}

	struct deduced_type {};

	template<typename T, typename U = deduced_type, typename Iterator,
	         typename OutputIterator, typename Function>
	[[maybe_unused]] void bit_cast_transform( Iterator first, Iterator last,
	                                          OutputIterator first_out,
	                                          Function &&func ) {

		using out_t = conditional_t<
		  std::is_same_v<U, deduced_type>,
		  typename std::iterator_traits<OutputIterator>::value_type, U>;

		while( first != last ) {
			*first_out =
			  DAW_BIT_CAST( out_t, daw::invoke( func, DAW_BIT_CAST( T, *first ) ) );
			++first;
			++first_out;
		}
	}

	template<typename T = deduced_type, typename Iterator,
	         typename OutputIterator>
	[[maybe_unused]] void bit_cast_copy( Iterator first, Iterator last,
	                                     OutputIterator first_out ) {
		using out_t = conditional_t<
		  std::is_same_v<T, deduced_type>,
		  typename std::iterator_traits<OutputIterator>::value_type, T>;

		while( first != last ) {
			*first_out = DAW_BIT_CAST( out_t, *first );
			++first;
			++first_out;
		}
	}

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
			[[maybe_unused]] static void test_aux( To ) noexcept;

			template<typename From, typename To>
			[[maybe_unused]] static bool_constant<
			  noexcept( test_aux<To>( std::declval<From>( ) ) )>
			test( int );

			template<typename, typename>
			[[maybe_unused]] static std::false_type test( ... );
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
	[[nodiscard, maybe_unused]] constexpr auto
	decay_copy( T &&v ) noexcept( is_nothrow_convertible_v<T, std::decay_t<T>> )
	  -> std::enable_if_t<std::is_convertible_v<T, std::decay_t<T>>,
	                      std::decay_t<T>> {

		return DAW_FWD( v );
	}

	template<typename Function, typename... Params>
	class bind_front {
		Function m_func;
		std::tuple<Params...> m_params;

	public:
		template<typename F, typename... P>
		explicit constexpr bind_front( F &&func, P &&...params )
		  : m_func( DAW_FWD( func ) )
		  , m_params( DAW_FWD( params )... ) {}

		template<typename... Args>
		constexpr decltype( auto ) operator( )( Args &&...args ) {
			static_assert( std::is_invocable_v<Function, Params..., Args...>,
			               "Arguments are not valid for function" );
			return std::apply(
			  m_func, std::tuple_cat( m_params,
			                          std::forward_as_tuple( DAW_FWD( args )... ) ) );
		}

		template<typename... Args>
		constexpr decltype( auto ) operator( )( Args &&...args ) const {
			static_assert( std::is_invocable_v<Function, Params..., Args...>,
			               "Arguments are not valid for function" );
			return std::apply(
			  m_func, std::tuple_cat( m_params,
			                          std::forward_as_tuple( DAW_FWD( args )... ) ) );
		}
	};

	template<typename Function, typename... Params>
	bind_front( Function, Params... ) -> bind_front<Function, Params...>;
} // namespace daw
#ifdef DAW_HAS_CONCEPTS
#undef DAW_HAS_CONCEPTS
#endif

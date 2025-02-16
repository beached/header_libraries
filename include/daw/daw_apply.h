// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_tuple_traits.h"

#include <type_traits>

namespace daw {
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

		template<typename F, typename... Args>
		[[nodiscard, maybe_unused]] constexpr decltype( auto ) INVOKE(
		  F &&f,
		  Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

			return DAW_FWD( f )( DAW_FWD( args )... );
		}
	} // namespace cpp_17_details

#if not defined( _MSC_VER ) or defined( __clang__ )
	template<typename F, typename... Args,
	         std::enable_if_t<sizeof...( Args ) != 1 or
	                            ( ( not is_reference_wrapper_v<Args> ) and ... ),
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr decltype( auto )
	invoke( F &&f,
	        Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

		return cpp_17_details::INVOKE( DAW_FWD( f ), DAW_FWD( args )... );
	}

	template<typename F, typename Arg, typename... Args>
	[[nodiscard]] constexpr decltype( auto ) invoke(
	  F &&f, std::reference_wrapper<Arg> arg,
	  std::reference_wrapper<
	    Args>... args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

		return cpp_17_details::INVOKE( DAW_FWD( f ), DAW_FWD( arg ),
		                               DAW_FWD( args )... );
	}
#else
	template<typename F, typename... Args>
	constexpr decltype( auto )
	invoke( F &&f,
	        Args &&...args ) noexcept( std::is_nothrow_invocable_v<F, Args...> ) {

		return DAW_FWD( f )( DAW_FWD( args )... );
	}
#endif

	namespace cpp_17_details {
		template<typename F, typename Tuple, std::size_t... I>
		[[nodiscard, maybe_unused]] constexpr decltype( auto )
		apply_details( F &&f, Tuple &&t, std::index_sequence<I...> ) {
			return daw::invoke( DAW_FWD( f ), std::get<I>( DAW_FWD( t ) )... );
		}

		template<typename F, typename Tuple, std::size_t... Is>
		[[nodiscard]] inline constexpr decltype( auto )
		apply_impl2( F &&f, Tuple &&t, std::index_sequence<Is...> ) {
			return DAW_FWD( f )( std::get<Is>( DAW_FWD( t ) )... );
		}
	} // namespace cpp_17_details

	template<bool use_invoke = true, typename F, typename Tuple>
	[[nodiscard, maybe_unused]] inline constexpr decltype( auto )
	apply( F &&f, Tuple &&t ) {
		static_assert( is_tuple_v<Tuple>,
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
} // namespace daw

// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <cstdlib>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

#include "cpp_17.h"
#include "daw_traits.h"

namespace daw {
	namespace get_nt_details {
		namespace gi_test {
			using namespace std;
			template<typename T>
			using get_if_test = decltype( get_if<0>( std::declval<T>( ) ) );
		} // namespace gi_test
		template<typename T>
		inline constexpr bool has_get_if_v =
		  daw::is_detected_v<gi_test::get_if_test, T>;
	} // namespace get_nt_details

	template<std::size_t Idx, typename Variant>
	constexpr decltype( auto ) get_nt( Variant &&var ) {
		if constexpr( get_nt_details::has_get_if_v<Variant> ) {
			auto *ptr = std::get_if<Idx>( &var );
			if constexpr( std::is_rvalue_reference_v<Variant> ) {
				return std::move( *ptr );
			} else {
				return *ptr;
			}
		} else {
			using namespace std;
			return get<Idx>( std::forward<Variant>( var ) );
		}
	}

	namespace visit_details {
		namespace test {
			struct Tst {
				[[maybe_unused]] constexpr void operator( )( int ) const {}
			};
		} // namespace test

		// A simple overload class that is constructed via aggregate.
		template<typename... Fs>
		struct overload_t : Fs... {
			using Fs::operator( )...;
		};
		template<typename... Fs>
		overload_t( Fs... ) -> overload_t<Fs...>;

		template<typename F, typename... Fs>
		[[nodiscard, maybe_unused]] constexpr decltype( auto )
		overload( F &&f, Fs &&... fs ) noexcept {
			if constexpr( sizeof...( Fs ) > 0 ) {
				return overload_t{
				  daw::traits::lift_func( std::forward<F>( f ) ),
				  daw::traits::lift_func( std::forward<Fs>( fs ) )... };
			} else {
				if constexpr( std::is_function_v<daw::remove_cvref_t<F>> ) {
					static_assert(
					  daw::traits::func_traits<F>::arity == 1,
					  "Function pointers only valid for functions with 1 argument" );
				}
				return daw::traits::lift_func( std::forward<F>( f ) );
			}
		}

		template<size_t N, typename R, typename Variant, typename Visitor>
		[[nodiscard]] constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
			if constexpr( N == 0 ) {
				// If this check isnt there the compiler will generate
				// exception code, this stops that
				if( N != var.index( ) or
				    not std::is_invocable_v<Visitor, decltype( get_nt<N>( var ) )> ) {
					std::abort( );
				}
				return std::forward<Visitor>( vis )(
				  get_nt<N>( std::forward<Variant>( var ) ) );
			} else {
				if( var.index( ) == N ) {
					if( not std::is_invocable_v<Visitor, decltype( get_nt<N>( var ) )> ) {
						std::abort( );
					}
					// If this check isnt there the compiler will generate
					// exception code, this stops that
					return std::forward<Visitor>( vis )(
					  get_nt<N>( std::forward<Variant>( var ) ) );
				}
				return visit_nt<N - 1, R>( std::forward<Variant>( var ),
				                           std::forward<Visitor>( vis ) );
			}
		}

		template<typename>
		struct get_var_size;

		template<template<class...> class Variant, typename... Args>
		struct get_var_size<Variant<Args...>> {
			constexpr static inline auto value = sizeof...( Args );
		};

		template<typename Variant>
		inline constexpr size_t get_var_size_v =
		  get_var_size<daw::remove_cvref_t<Variant>>::value;

	} // namespace visit_details
	//**********************************************

	// Singe visitation visit.  Expects that variant is valid and not empty
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get_nt<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr decltype( auto )
	visit_nt( Variant &&var, Visitors &&... visitors ) {

		constexpr size_t var_sz = daw::visit_details::get_var_size_v<Variant>;
		static_assert( var_sz > 0 );
		using result_t =
		  decltype( daw::visit_details::overload( std::forward<Visitors>(
		    visitors )... )( get_nt<0>( std::forward<Variant>( var ) ) ) );

		return daw::visit_details::visit_nt<var_sz - 1, result_t>(
		  std::forward<Variant>( var ),
		  daw::visit_details::overload( std::forward<Visitors>( visitors )... ) );
	}

	// Singe visitation visit with user choosable result.  Expects that variant is
	// valid and not empty
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr Result
	visit_nt( Variant &&var, Visitors &&... visitors ) {

		constexpr size_t var_sz = daw::visit_details::get_var_size_v<Variant>;
		static_assert( var_sz > 0 );

		return daw::visit_details::visit_nt<var_sz - 1, Result>(
		  std::forward<Variant>( var ),
		  daw::visit_details::overload( std::forward<Visitors>( visitors )... ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool
	  is_visitable_v = ( std::is_invocable_v<Visitors, Value> or ... );
} // namespace daw

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
#include "daw_assume.h"
#include "daw_attributes.h"
#include "daw_check_exceptions.h"
#include "daw_likely.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "daw_unreachable.h"

#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>
#include <variant>

namespace daw {
	using bad_variant_access = std::bad_variant_access;
	//vtable is showing up in code
	//struct bad_variant_access : std::exception {};

	namespace get_nt_details {
		namespace gi_test {
			using namespace std;
			template<typename T>
			using get_if_test = decltype( get_if<0>( std::declval<T *>( ) ) );

			template<typename T>
			using index_test = decltype( std::declval<T>( ).index( ) );
		} // namespace gi_test
		template<typename T>
		inline constexpr bool has_get_if_v =
		  daw::is_detected_v<gi_test::get_if_test, T>;

		template<typename T>
		inline constexpr bool has_index_v =
		  daw::is_detected_v<gi_test::index_test, T>;
	} // namespace get_nt_details

	template<std::size_t Idx, typename Variant>
	constexpr decltype( auto ) get_nt( Variant &&var ) {
		using namespace std;
		if constexpr( get_nt_details::has_get_if_v<
		                std::remove_reference_t<Variant>> ) {
			auto *ptr = get_if<Idx>( &var );
			DAW_ASSUME( ptr != nullptr );
			if constexpr( std::is_rvalue_reference_v<Variant> ) {
				return DAW_MOVE( *ptr );
			} else {
				return *ptr;
			}
		} else {
			return get<Idx>( DAW_FWD2( Variant, var ) );
		}
	}
	namespace visit_details {
		template<typename Variant>
		constexpr auto get_index( Variant const &v ) {
			return v.index( );
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

		// A simple overload class that is constructed via aggregate.
		template<typename... Fs>
		struct overload_t : Fs... {
			using Fs::operator( )...;
		};
		template<typename... Fs>
		overload_t( Fs... ) -> overload_t<Fs...>;

		template<typename F, typename... Fs>
		[[nodiscard, maybe_unused]] constexpr decltype( auto )
		overload( F &&f, Fs &&...fs ) noexcept {
			if constexpr( sizeof...( Fs ) > 0 ) {
				return overload_t{ daw::traits::lift_func( DAW_FWD2( F, f ) ),
				                   daw::traits::lift_func( DAW_FWD2( Fs, fs ) )... };
			} else {
				if constexpr( std::is_function_v<daw::remove_cvref_t<F>> ) {
					static_assert(
					  daw::traits::func_traits<F>::arity == 1,
					  "Function pointers only valid for functions with 1 argument" );
				}
				return daw::traits::lift_func( DAW_FWD2( F, f ) );
			}
		}

		template<std::size_t N, typename R, typename Variant, typename Visitor>
		[[nodiscard]] constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
			constexpr std::size_t VSz = get_var_size_v<Variant>;
			if constexpr( VSz - N >= 8 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				case 3 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<3 + N>( DAW_FWD2( Variant, var ) ) );
				case 4 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<4 + N>( DAW_FWD2( Variant, var ) ) );
				case 5 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<5 + N>( DAW_FWD2( Variant, var ) ) );
				case 6 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<6 + N>( DAW_FWD2( Variant, var ) ) );
				case 7 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<7 + N>( DAW_FWD2( Variant, var ) ) );
				default:
					if constexpr( VSz - N > 8 ) {
						return visit_nt<N + 8, R>( DAW_FWD2( Variant, var ),
						                           DAW_FWD2( Visitor, vis ) );
					} else {
						DAW_UNREACHABLE( );
					}
				}
			} else if constexpr( VSz - N == 7 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				case 3 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<3 + N>( DAW_FWD2( Variant, var ) ) );
				case 4 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<4 + N>( DAW_FWD2( Variant, var ) ) );
				case 5 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<5 + N>( DAW_FWD2( Variant, var ) ) );
				case 6 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<6 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else if constexpr( VSz - N == 6 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				case 3 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<3 + N>( DAW_FWD2( Variant, var ) ) );
				case 4 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<4 + N>( DAW_FWD2( Variant, var ) ) );
				case 5 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<5 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else if constexpr( VSz - N == 5 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				case 3 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<3 + N>( DAW_FWD2( Variant, var ) ) );
				case 4 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<4 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else if constexpr( VSz - N == 4 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				case 3 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<3 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else if constexpr( VSz - N == 3 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				case 2 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<2 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else if constexpr( VSz - N == 2 ) {
				switch( get_index( var ) ) {
				case 0 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
				case 1 + N:
					return DAW_FWD2( Visitor,
					                 vis )( get_nt<1 + N>( DAW_FWD2( Variant, var ) ) );
				}
			} else {
				return DAW_FWD2( Visitor,
				                 vis )( get_nt<0 + N>( DAW_FWD2( Variant, var ) ) );
			}
			DAW_UNREACHABLE( );
		}

		[[noreturn, maybe_unused]] DAW_ATTRIB_NOINLINE inline void visit_error( ) {
			DAW_THROW_OR_TERMINATE_NA( daw::bad_variant_access );
		}
	} // namespace visit_details
	//**********************************************

	// Singe visitation visit.  Expects that variant is valid and not empty
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get_nt<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr decltype( auto )
	visit_nt( Variant &&var, Visitors &&...visitors ) {
		using result_t = decltype( daw::visit_details::overload( DAW_FWD2(
		  Visitors, visitors )... )( get_nt<0>( DAW_FWD2( Variant, var ) ) ) );

		return daw::visit_details::visit_nt<0, result_t>(
		  DAW_FWD2( Variant, var ),
		  daw::visit_details::overload( DAW_FWD2( Visitors, visitors )... ) );
	}

	// Singe visitation visit.
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get_nt<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr decltype( auto )
	visit( Variant &&var, Visitors &&...visitors ) {
		using result_t = decltype( daw::visit_details::overload( DAW_FWD2(
		  Visitors, visitors )... )( get_nt<0>( DAW_FWD2( Variant, var ) ) ) );

		if( DAW_UNLIKELY( var.index( ) < 0 ) ) {
			visit_details::visit_error( );
		}
		return daw::visit_details::visit_nt<0, result_t>(
		  DAW_FWD2( Variant, var ),
		  daw::visit_details::overload( DAW_FWD2( Visitors, visitors )... ) );
	}

	// Singe visitation visit with user choosable result.
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr Result visit( Variant &&var,
	                                                    Visitors &&...visitors ) {

		if( DAW_UNLIKELY( var.index( ) < 0 ) ) {
			visit_details::visit_error( );
		}
		return daw::visit_details::visit_nt<0, Result>(
		  DAW_FWD2( Variant, var ),
		  daw::visit_details::overload( DAW_FWD2( Visitors, visitors )... ) );
	}

	// Singe visitation visit with user choosable result.  Expects that variant is
	// valid and not empty
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr Result
	visit_nt( Variant &&var, Visitors &&...visitors ) {
		return daw::visit_details::visit_nt<0, Result>(
		  DAW_FWD2( Variant, var ),
		  daw::visit_details::overload( DAW_FWD2( Visitors, visitors )... ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool is_visitable_v =
	  ( std::is_invocable_v<Visitors, Value> or ... );
} // namespace daw

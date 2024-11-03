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
#include "daw/daw_check_exceptions.h"
#include "daw_consteval.h"
#include "daw_likely.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "daw_unreachable.h"
#include "impl/daw_make_trait.h"

#include <cstddef>
#include <daw/stdinc/move_fwd_exch.h>
#include <exception>
#include <type_traits>
#include <variant>

namespace daw {
	using bad_variant_access = std::bad_variant_access;
	// vtable is showing up in code
	// struct bad_variant_access : std::exception {};

	namespace get_nt_details {
		// These ensure there exists something to test against when it isn't
		// available prior to c++20
		template<std::size_t>
		void get_if( );

		template<std::size_t>
		void get( );

		DAW_MAKE_REQ_TRAIT( has_adl_get_if_v, get_if<0>( std::declval<T *>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_std_get_if_v,
		                    std::get_if<0>( std::declval<T *>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_adl_get_v, get<0>( std::declval<T>( ) ) );

		DAW_MAKE_REQ_TRAIT( has_std_get_v, std::get<0>( std::declval<T>( ) ) );
	} // namespace get_nt_details

	template<std::size_t Idx, typename Variant>
	DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get_nt( Variant &&var ) {
		using variant_t = std::remove_reference_t<Variant>;
		if constexpr( get_nt_details::has_adl_get_if_v<variant_t> ) {
			using get_nt_details::get_if;
			auto *ptr = get_if<Idx>( std::addressof( var ) );
			DAW_ASSUME( ptr != nullptr );
			if constexpr( std::is_rvalue_reference_v<Variant> ) {
				return std::move( *ptr );
			} else {
				return *ptr;
			}
		} else if constexpr( get_nt_details::has_std_get_if_v<variant_t> ) {
			auto *ptr = std::get_if<Idx>( std::addressof( var ) );
			DAW_ASSUME( ptr != nullptr );
			if constexpr( std::is_rvalue_reference_v<Variant> ) {
				return std::move( *ptr );
			} else {
				return *ptr;
			}
		} else if constexpr( get_nt_details::has_adl_get_v<variant_t> ) {
			using get_nt_details::get;
			return get<Idx>( DAW_FWD( var ) );
		} else {
			static_assert( get_nt_details::has_std_get_v<variant_t> );
			return std::get<Idx>( DAW_FWD( var ) );
		}
	}

	namespace visit_details {
		template<typename Variant>
		DAW_ATTRIB_FLATINLINE constexpr auto get_index( Variant const &v ) {
			return v.index( );
		}

	  DAW_MAKE_REQ_TRAIT( has_variant_npos, T::variant_npos );
	  
    template<typename Variant, std::enable_if_t<has_variant_npos<Variant>, std::nullptr_t> = nullptr> 
	  DAW_ATTRIB_FLATINLINE constexpr bool is_empty( Variant const & v ) {
		  return v.index( ) == Variant::variant_npos;
		}

    template<typename Variant, std::enable_if_t<not has_variant_npos<Variant>, std::nullptr_t> = nullptr> 
	  DAW_ATTRIB_INLINE constexpr bool is_empty( Variant const & ) {
      return false;
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
		DAW_ATTRIB_FLATINLINE [[nodiscard]] constexpr decltype( auto )
		overload( F &&f, Fs &&...fs ) noexcept {
			if constexpr( sizeof...( Fs ) > 0 ) {
				return overload_t{ daw::traits::lift_func( DAW_FWD( f ) ),
				                   daw::traits::lift_func( DAW_FWD( fs ) )... };
			} else {
				if constexpr( std::is_function_v<daw::remove_cvref_t<F>> ) {
					static_assert(
					  daw::traits::func_traits<F>::arity == 1,
					  "Function pointers only valid for functions with 1 argument" );
				}
				return daw::traits::lift_func( DAW_FWD( f ) );
			}
		}

#define DAW_VISIT_CASE( Base, Idx ) \
	case Base + Idx:                  \
		return DAW_FWD( vis )( get_nt<Base + Idx>( DAW_FWD( var ) ) )

		template<std::size_t N, typename R, typename Variant, typename Visitor>
		DAW_ATTRIB_FLATINLINE [[nodiscard]] inline constexpr R
		visit_nt( Variant &&var, Visitor &&vis ) {
			constexpr std::size_t VSz = get_var_size_v<Variant>;
			if constexpr( VSz - N >= 16 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
					DAW_VISIT_CASE( N, 11 );
					DAW_VISIT_CASE( N, 12 );
					DAW_VISIT_CASE( N, 13 );
					DAW_VISIT_CASE( N, 14 );
					DAW_VISIT_CASE( N, 15 );
				default:
					if constexpr( VSz - N > 16 ) {
						return visit_nt<N + 16, R>( DAW_FWD( var ), DAW_FWD( vis ) );
					} else {
						DAW_UNREACHABLE( );
					}
				}
			} else if constexpr( VSz - N == 15 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
					DAW_VISIT_CASE( N, 11 );
					DAW_VISIT_CASE( N, 12 );
					DAW_VISIT_CASE( N, 13 );
					DAW_VISIT_CASE( N, 14 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 14 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
					DAW_VISIT_CASE( N, 11 );
					DAW_VISIT_CASE( N, 12 );
					DAW_VISIT_CASE( N, 13 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 13 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
					DAW_VISIT_CASE( N, 11 );
					DAW_VISIT_CASE( N, 12 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 12 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
					DAW_VISIT_CASE( N, 11 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N >= 11 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
					DAW_VISIT_CASE( N, 10 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 10 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
					DAW_VISIT_CASE( N, 9 );
				}
			} else if constexpr( VSz - N == 9 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
					DAW_VISIT_CASE( N, 8 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 8 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
					DAW_VISIT_CASE( N, 7 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 7 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
					DAW_VISIT_CASE( N, 6 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 6 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
					DAW_VISIT_CASE( N, 5 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 5 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
					DAW_VISIT_CASE( N, 4 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 4 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
					DAW_VISIT_CASE( N, 3 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 3 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
					DAW_VISIT_CASE( N, 2 );
				default:
					DAW_UNREACHABLE( );
				}
			} else if constexpr( VSz - N == 2 ) {
				switch( get_index( var ) ) {
					DAW_VISIT_CASE( N, 0 );
					DAW_VISIT_CASE( N, 1 );
				default:
					DAW_UNREACHABLE( );
				}
			} else {
				return DAW_FWD( vis )( get_nt<0 + N>( DAW_FWD( var ) ) );
			}
			DAW_UNREACHABLE( );
		}

#undef DAW_VISIT_CASE

		[[noreturn]] DAW_ATTRIB_NOINLINE inline void visit_error( ) {
			DAW_THROW_OR_TERMINATE_NA( daw::bad_variant_access );
		}
	} // namespace visit_details
	//**********************************************

	// Singe visitation visit.  Expects that variant is valid and not empty
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get_nt<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard]] constexpr decltype( auto ) visit_nt( Variant &&var,
	                                                   Visitors &&...visitors ) {
		using result_t = decltype( daw::visit_details::overload(
		  DAW_FWD( visitors )... )( get_nt<0>( DAW_FWD( var ) ) ) );

		return daw::visit_details::visit_nt<0, result_t>(
		  DAW_FWD( var ), daw::visit_details::overload( DAW_FWD( visitors )... ) );
	}

	// Singe visitation visit.
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get_nt<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard]] constexpr decltype( auto ) visit( Variant &&var,
	                                                Visitors &&...visitors ) {
		using result_t = decltype( daw::visit_details::overload(
		  DAW_FWD( visitors )... )( get_nt<0>( DAW_FWD( var ) ) ) );

		if( DAW_UNLIKELY( visit_details::is_empty( var ) ) ) {
		  DAW_UNLIKELY_BRANCH
			visit_details::visit_error( );
		}
		return daw::visit_details::visit_nt<0, result_t>(
		  DAW_FWD( var ), daw::visit_details::overload( DAW_FWD( visitors )... ) );
	}

	// Singe visitation visit with user choosable result.
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard]] constexpr Result visit( Variant &&var,
	                                      Visitors &&...visitors ) {

		if( DAW_UNLIKELY( var.index( ) < 0 ) ) {
			visit_details::visit_error( );
		}
		return daw::visit_details::visit_nt<0, Result>(
		  DAW_FWD( var ), daw::visit_details::overload( DAW_FWD( visitors )... ) );
	}

	// Singe visitation visit with user choosable result.  Expects that variant is
	// valid and not empty
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard]] constexpr Result visit_nt( Variant &&var,
	                                         Visitors &&...visitors ) {
		return daw::visit_details::visit_nt<0, Result>(
		  DAW_FWD( var ), daw::visit_details::overload( DAW_FWD( visitors )... ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool is_visitable_v =
	  ( std::is_invocable_v<Visitors, Value> or ... );
} // namespace daw

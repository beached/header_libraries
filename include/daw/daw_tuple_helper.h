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
#include "daw_math.h"
#include "daw_move.h"
#include "daw_overload.h"
#include "daw_utility.h"
#include "traits/daw_traits_conditional.h"

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>
#include <daw/stdinc/reference_wrapper.h>
#include <iostream>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <variant>

namespace daw {
	namespace tuple {
		namespace tuple_details {
			template<size_t Tpos, typename Result, typename Tuple,
			         std::enable_if_t<
			           (Tpos >= std::tuple_size_v<std::remove_reference_t<Tuple>>),
			           std::nullptr_t> = nullptr>
			[[noreturn]] inline Result get_impl( Tuple &&, size_t ) noexcept {
				std::abort( );
			}

			template<size_t Tpos, typename Result, typename Tuple,
			         std::enable_if_t<
			           (Tpos < std::tuple_size_v<std::remove_reference_t<Tuple>>),
			           std::nullptr_t> = nullptr>
			constexpr Result get_impl( Tuple &&tp, size_t index ) noexcept {
				if( index != Tpos ) {
					return get_impl<Tpos + 1, Result>( DAW_FWD2( Tuple, tp ), index );
				}
				return daw::construct_a<Result>(
				  std::ref( std::get<Tpos>( DAW_FWD2( Tuple, tp ) ) ) );
			}
		} // namespace tuple_details
	} // namespace tuple

	template<typename... Args>
	constexpr auto get( std::tuple<Args...> &tp, size_t index ) noexcept {
		using result_t =
		  std::variant<std::reference_wrapper<std::remove_reference_t<Args>>...>;
		return daw::tuple::tuple_details::get_impl<0, result_t>( tp, index );
	}

	template<typename... Args>
	constexpr auto get( std::tuple<Args...> const &tp, size_t index ) noexcept {
		using result_t = std::variant<
		  std::reference_wrapper<std::remove_reference_t<Args> const>...>;
		return daw::tuple::tuple_details::get_impl<0, result_t>( tp, index );
	}

	namespace tuple {
		namespace tuple_details {
			template<typename T1, typename F>
			constexpr bool for_each( T1 &&t1, F &&f ) {
				::Unused( ( daw::invoke( DAW_FWD2( F, f ), DAW_FWD2( T1, t1 ) ), 0 ) );
				return true;
			}

			template<typename T1, typename F, size_t... Is>
			constexpr void for_each( T1 &&t1, F &&f, std::index_sequence<Is...> ) {
				::Unused(
				  ( for_each( std::get<Is>( DAW_FWD2( T1, t1 ) ), f ) && ... ) );
			}

			struct min_t {
				template<typename Result, typename A, typename B>
				constexpr void operator( )( Result &result, A const &a,
				                            B const &b ) const {
					using daw::min;
					result = (min)( a, b );
				}

				static min_t const &get( ) noexcept {
					static min_t result{ };
					return result;
				}
			}; // min_t

			struct max_t {
				template<typename Result, typename A, typename B>
				constexpr void operator( )( Result &result, A const &a,
				                            B const &b ) const {
					using daw::max;
					result = (max)( a, b );
				}

				static max_t const &get( ) noexcept {
					static max_t result{ };
					return result;
				}
			}; // max_t

			using namespace std::literals::string_literals;

			class print_t {
				std::ostream &m_os;
				bool m_is_first;
				char m_separator;

			public:
				constexpr print_t( std::ostream &os, char separator = ',' )
				  : m_os{ os }
				  , m_is_first{ true }
				  , m_separator{ std::move( separator ) } {}

				void reset( ) {
					m_is_first = true;
				}
				template<typename T>
				constexpr void operator( )( T const &v ) {
					if( !m_is_first ) {
						m_os << m_separator;
					} else {
						m_is_first = false;
					}
					m_os << " " << v;
				}
			}; // print_t
		} // namespace tuple_details

		template<typename... Ts, typename F>
		constexpr void for_each( std::tuple<Ts...> const &t1, F &&f ) {
			tuple_details::for_each( t1, DAW_FWD2( F, f ),
			                         std::index_sequence_for<Ts...>{ } );
		}

		template<typename... Ts, typename F>
		constexpr void apply( std::tuple<Ts...> &t1, F &&f ) {
			tuple_details::for_each( t1, DAW_FWD2( F, f ),
			                         std::index_sequence_for<Ts...>{ } );
		}

		namespace operators {
			namespace tuple_details {
				template<size_t I, typename Result, typename T1, typename T2,
				         typename F>
				constexpr bool apply_tuple( Result &&result, T1 &&op1, T2 &&op2,
				                            F &&f ) {
					::Unused( ( daw::invoke( DAW_FWD2( F, f ), std::get<I>( result ),
					                         std::get<I>( op1 ), std::get<I>( op2 ) ),
					            0 ) );
					return true;
				}

				template<typename Result, typename T1, typename T2, typename F,
				         size_t... Is>
				constexpr void apply_tuple( Result &&result, T1 &&op1, T2 &&op2, F &&f,
				                            std::index_sequence<Is...> ) {

					::Unused( ( apply_tuple<Is>( result, op1, op2, f ) && ... ) );
				}

				template<size_t I, typename Result, typename T1, typename T, typename F>
				constexpr bool apply_value( Result &&result, T1 &&op1, T &&op2,
				                            F &&f ) {
					::Unused(
					  ( daw::invoke( f, std::get<I>( result ), std::get<I>( op1 ), op2 ),
					    0 ) );
					return true;
				}

				template<typename Result, typename T1, typename T, typename F,
				         size_t... Is>
				constexpr void apply_value( Result &&result, T1 &&op1, T &&op2, F &&f,
				                            std::index_sequence<Is...> ) {

					::Unused( ( apply_value<Is>( result, op1, op2, f ) && ... ) );
				}

				template<size_t I, typename Result, typename T1, typename T, typename F>
				constexpr bool apply_value2( Result &&result, T1 &&op1, T &&op2,
				                             F &&f ) {
					::Unused( ( daw::invoke( DAW_FWD2( F, f ), std::get<I>( result ), op1,
					                         std::get<I>( op2 ) ),
					            0 ) );
					return true;
				}

				template<typename Result, typename T1, typename T, typename F,
				         size_t... Is>
				constexpr void apply_value2( Result &&result, T1 &&op1, T &&op2, F &&f,
				                             std::index_sequence<Is...> ) {
					::Unused( ( apply_value2<Is>( result, op1, op2, f ) && ... ) );
				}

				template<typename... Ts, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_tuple( std::tuple<Ts...> const &op1,
				                   std::tuple<Ts...> const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_tuple( result, op1, op2, f, std::index_sequence_for<Ts...>{ } );
					return result;
				}

				template<typename... Ts, typename T, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_value2( T const &op1, std::tuple<Ts...> const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_value2( result, op1, op2, f,
					              std::index_sequence_for<Ts...>{ } );
					return result;
				}

				template<typename... Ts, typename T, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_value( std::tuple<Ts...> const &op1, T const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_value( result, op1, op2, f, std::index_sequence_for<Ts...>{ } );
					return result;
				}

				struct add_t {
					template<typename Result, typename Lhs, typename Rhs>
					constexpr void operator( )( Result &result, Lhs const &lhs,
					                            Rhs const &rhs ) const {
						result = lhs + rhs;
					}

					static add_t const &get( ) {
						static add_t result{ };
						return result;
					}
				}; // add_t

				struct sub_t {
					template<typename Result, typename Lhs, typename Rhs>
					constexpr void operator( )( Result &result, Lhs const &lhs,
					                            Rhs const &rhs ) const {
						result = lhs - rhs;
					}

					static sub_t const &get( ) {
						static sub_t result{ };
						return result;
					}
				}; // sub_t

				struct mul_t {
					template<typename Result, typename Lhs, typename Rhs>
					constexpr void operator( )( Result &result, Lhs const &lhs,
					                            Rhs const &rhs ) const {
						result = lhs * rhs;
					}

					static mul_t const &get( ) {
						static mul_t result{ };
						return result;
					}
				}; // mul_t

				struct div_t {
					template<typename Result, typename Lhs, typename Rhs>
					constexpr void operator( )( Result &result, Lhs const &lhs,
					                            Rhs const &rhs ) const {
						result = lhs / rhs;
					}

					static div_t const &get( ) {
						static div_t result{ };
						return result;
					}
				}; // div_t
			} // namespace tuple_details

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator+( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return tuple_details::apply_tuple_tuple( lhs, rhs,
				                                         tuple_details::add_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator+( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return tuple_details::apply_tuple_value2(
				  lhs, rhs, tuple_details::add_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator+( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return tuple_details::apply_tuple_value( lhs, rhs,
				                                         tuple_details::add_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator-( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return tuple_details::apply_tuple_tuple( lhs, rhs,
				                                         tuple_details::sub_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator-( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return tuple_details::apply_tuple_value2(
				  lhs, rhs, tuple_details::sub_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator-( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return tuple_details::apply_tuple_value( lhs, rhs,
				                                         tuple_details::sub_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator*( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return tuple_details::apply_tuple_tuple( lhs, rhs,
				                                         tuple_details::mul_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator*( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return tuple_details::apply_tuple_value2(
				  lhs, rhs, tuple_details::mul_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator*( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return tuple_details::apply_tuple_value( lhs, rhs,
				                                         tuple_details::mul_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator/( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return tuple_details::apply_tuple_tuple( lhs, rhs,
				                                         tuple_details::div_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator/( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return tuple_details::apply_tuple_value2(
				  lhs, rhs, tuple_details::div_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator/( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return tuple_details::apply_tuple_value( lhs, rhs,
				                                         tuple_details::div_t::get( ) );
			}
		} // namespace operators

		template<typename... Ts>
		inline std::ostream &operator<<( std::ostream &os,
		                                 std::tuple<Ts...> const &t ) {
			os << "{";
			for_each( t, tuple_details::print_t{ os } );
			os << " }";
			return os;
		}

		template<typename... Ts>
		constexpr auto( min )( std::tuple<Ts...> const &a,
		                       std::tuple<Ts...> const &b ) {
			return operators::tuple_details::apply_tuple_tuple(
			  a, b, daw::tuple::tuple_details::min_t::get( ) );
		}

		template<typename... Ts>
		constexpr auto( max )( std::tuple<Ts...> const &a,
		                       std::tuple<Ts...> const &b ) {
			return operators::tuple_details::apply_tuple_tuple(
			  a, b, daw::tuple::tuple_details::max_t::get( ) );
		}

		namespace tuple_details {
			template<typename Func>
			constexpr void apply_func_impl( Func &&, ... ) noexcept {}

			template<typename Func, typename Arg>
			constexpr void apply_func_impl( Func &&func, Arg &&arg ) noexcept(
			  std::is_nothrow_invocable_v<Func, Arg> ) {

				if constexpr( std::is_invocable_v<Func, Arg> ) {
					(void)daw::invoke( DAW_FWD2( Func, func ), DAW_FWD2( Arg, arg ) );
				}
			}

			template<typename Func>
			struct call_func {
				Func func;

				template<typename Arg, std::enable_if_t<std::is_invocable_v<Func, Arg>,
				                                        std::nullptr_t> = nullptr>
				constexpr void
				operator( )( Arg &&value ) noexcept( noexcept( func( value ) ) ) {
					func( DAW_FWD2( Arg, value ) );
				}

				constexpr void operator( )( ... ) noexcept {}
			};

			template<typename Func>
			constexpr call_func<Func> make_call_func( Func &&func ) noexcept {
				return call_func<Func>{ DAW_FWD2( Func, func ) };
			}

			template<size_t N, typename Func, typename Arg>
			constexpr bool apply_func( size_t idx, Func func, Arg &&arg ) noexcept(
			  noexcept( apply_func_impl( std::move( func ),
			                             DAW_FWD2( Arg, arg ) ) ) ) {

				if( N == idx ) {
					apply_func_impl( std::move( func ), DAW_FWD2( Arg, arg ) );
				}
				return true;
			}

			template<typename Tuple, typename Func, size_t... Is>
			constexpr void apply_at_impl( Tuple &&tp, size_t index, Func &&func,
			                              std::index_sequence<Is...> ) {
				Unused( ( apply_func<Is>( index, func,
				                          std::get<Is>( DAW_FWD2( Tuple, tp ) ) ) and
				          ... ) );
			}
		} // namespace tuple_details

		template<typename... Args, typename... Funcs>
		constexpr void apply_at( std::tuple<Args...> const &tp, size_t index,
		                         Funcs &&...funcs ) {
			tuple_details::apply_at_impl(
			  tp, index, daw::overload( DAW_FWD2( Funcs, funcs )... ),
			  std::index_sequence_for<Args...>{ } );
		}

		template<typename... Args, typename... Funcs>
		constexpr void apply_at( std::tuple<Args...> &tp, size_t index,
		                         Funcs &&...funcs ) {
			tuple_details::apply_at_impl(
			  tp, index, daw::overload( DAW_FWD2( Funcs, funcs )... ),
			  std::index_sequence_for<Args...>{ } );
		}

		template<typename... Args, typename... Funcs>
		constexpr void apply_at( std::tuple<Args...> &&tp, size_t index,
		                         Funcs &&...funcs ) {
			tuple_details::apply_at_impl(
			  std::move( tp ), index, daw::overload( DAW_FWD2( Funcs, funcs )... ) );
		}

		namespace tuple_details {
			template<class T, class Tuple, std::size_t... I>
			constexpr T make_from_tuple_impl( Tuple &&t, std::index_sequence<I...>,
			                                  std::true_type ) {
				return T{ std::get<I>( DAW_FWD2( Tuple, t ) )... };
			}

			template<class T, class Tuple, std::size_t... I>
			constexpr T make_from_tuple_impl( Tuple &&t, std::index_sequence<I...>,
			                                  std::false_type ) {
				return T( std::get<I>( DAW_FWD2( Tuple, t ) )... );
			}

			template<typename... Args>
			constexpr std::integral_constant<size_t, sizeof...( Args )>
			tuple_size_test( std::tuple<Args...> const && ) noexcept;
			template<typename... Args>
			constexpr std::integral_constant<size_t, sizeof...( Args )>
			tuple_size_test( std::tuple<Args...> const volatile && ) noexcept;
			template<typename... Args>
			constexpr std::integral_constant<size_t, sizeof...( Args )>
			tuple_size_test( std::tuple<Args...> const & ) noexcept;
			template<typename... Args>
			constexpr std::integral_constant<size_t, sizeof...( Args )>
			tuple_size_test( std::tuple<Args...> const volatile & ) noexcept;
		} // namespace tuple_details
	} // namespace tuple

	template<class T, class Tuple>
	constexpr T make_from_tuple2( Tuple &&t ) {
		return tuple::tuple_details::make_from_tuple_impl<T>(
		  DAW_FWD2( Tuple, t ),
		  std::make_index_sequence<
		    std::tuple_size_v<std::remove_reference_t<Tuple>>>{ },
		  std::is_aggregate<T>{ } );
	}

	template<typename Tuple>
	inline constexpr size_t tuple_size_v =
	  decltype( tuple::tuple_details::tuple_size_test(
	    std::declval<Tuple>( ) ) )::value;

	template<typename Tuple>
	inline constexpr bool is_empty_tuple_v = ( tuple_size_v<Tuple> == 0 );

	template<typename... Ts>
	constexpr auto forward_nontemp_as_tuple( Ts &&...values ) {
		using tuple_t = std::tuple<conditional_t<std::is_rvalue_reference_v<Ts>,
		                                         daw::remove_cvref_t<Ts>, Ts>...>;
		return tuple_t{ DAW_FWD( values )... };
	}
} // namespace daw

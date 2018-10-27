// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
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

#include <cstddef>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "cpp_17.h"
#include "daw_math.h"
#include "daw_static_array.h"

namespace daw {
	namespace tuple {
		namespace detail {
			template<typename T>
			constexpr void do_nothing( T && ) noexcept {}

			template<int... Is>
			struct seq {};

			template<int N, int... Is>
			struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

			template<int... Is>
			struct gen_seq<0, Is...> : seq<Is...> {};

			template<typename T1, typename F, int... Is>
			constexpr void for_each( T1 &&t1, F &&f, seq<Is...> ) {
				auto l = {( f( std::get<Is>( std::forward<T1>( t1 ) ) ), 0 )...};
				do_nothing( l );
			}

			struct min_t {
				template<typename Result, typename A, typename B>
				constexpr void operator( )( Result &result, A const &a,
				                            B const &b ) const {
					using daw::min;
					result = min( a, b );
				}

				static min_t const &get( ) noexcept {
					static min_t result{};
					return result;
				}
			}; // min_t

			struct max_t {
				template<typename Result, typename A, typename B>
				constexpr void operator( )( Result &result, A const &a,
				                            B const &b ) const {
					using daw::max;
					result = max( a, b );
				}

				static max_t const &get( ) noexcept {
					static max_t result{};
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
				  : m_os{os}
				  , m_is_first{true}
				  , m_separator{std::move( separator )} {}

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
		}    // namespace detail

		template<typename... Ts, typename F>
		constexpr void for_each( std::tuple<Ts...> const &t1, F &&f ) {
			detail::for_each( t1, std::forward<F>( f ),
			                  detail::gen_seq<sizeof...( Ts )>( ) );
		}

		template<typename... Ts, typename F>
		constexpr void apply( std::tuple<Ts...> &t1, F &&f ) {
			detail::for_each( t1, std::forward<F>( f ),
			                  detail::gen_seq<sizeof...( Ts )>( ) );
		}

		namespace operators {
			namespace detail {
				template<typename Result, typename T1, typename T2, typename F,
				         int... Is>
				constexpr void apply_tuple( Result &&result, T1 &&op1, T2 &&op2, F f,
				                            daw::tuple::detail::seq<Is...> ) {
					auto l = {( f( std::get<Is>( result ), std::get<Is>( op1 ),
					               std::get<Is>( op2 ) ),
					            0 )...};
					daw::tuple::detail::do_nothing( l );
				}

				template<typename Result, typename T1, typename T, typename F,
				         int... Is>
				constexpr void apply_value( Result &&result, T1 &&op1, T &&op2, F f,
				                            daw::tuple::detail::seq<Is...> ) {
					auto l = {
					  ( f( std::get<Is>( result ), std::get<Is>( op1 ), op2 ), 0 )...};
					daw::tuple::detail::do_nothing( l );
				}

				template<typename Result, typename T1, typename T, typename F,
				         int... Is>
				constexpr void apply_value2( Result &&result, T1 &&op1, T &&op2, F f,
				                             daw::tuple::detail::seq<Is...> ) {
					auto l = {
					  ( f( std::get<Is>( result ), op1, std::get<Is>( op2 ) ), 0 )...};
					daw::tuple::detail::do_nothing( l );
				}

				template<typename... Ts, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_tuple( std::tuple<Ts...> const &op1,
				                   std::tuple<Ts...> const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_tuple( result, op1, op2, f,
					             daw::tuple::detail::gen_seq<sizeof...( Ts )>( ) );
					return result;
				}

				template<typename... Ts, typename T, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_value2( T const &op1, std::tuple<Ts...> const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_value2( result, op1, op2, f,
					              daw::tuple::detail::gen_seq<sizeof...( Ts )>( ) );
					return result;
				}

				template<typename... Ts, typename T, typename F>
				constexpr std::tuple<Ts...>
				apply_tuple_value( std::tuple<Ts...> const &op1, T const &op2, F f ) {
					std::tuple<Ts...> result;
					apply_value( result, op1, op2, f,
					             daw::tuple::detail::gen_seq<sizeof...( Ts )>( ) );
					return result;
				}

				struct add_t {
					template<typename Result, typename Lhs, typename Rhs>
					constexpr void operator( )( Result &result, Lhs const &lhs,
					                            Rhs const &rhs ) const {
						result = lhs + rhs;
					}

					static add_t const &get( ) {
						static add_t result{};
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
						static sub_t result{};
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
						static mul_t result{};
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
						static div_t result{};
						return result;
					}
				}; // div_t
			}    // namespace detail

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator+( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return detail::apply_tuple_tuple( lhs, rhs, detail::add_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator+( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return detail::apply_tuple_value2( lhs, rhs, detail::add_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator+( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return detail::apply_tuple_value( lhs, rhs, detail::add_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator-( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return detail::apply_tuple_tuple( lhs, rhs, detail::sub_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator-( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return detail::apply_tuple_value2( lhs, rhs, detail::sub_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator-( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return detail::apply_tuple_value( lhs, rhs, detail::sub_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator*( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return detail::apply_tuple_tuple( lhs, rhs, detail::mul_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator*( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return detail::apply_tuple_value2( lhs, rhs, detail::mul_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator*( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return detail::apply_tuple_value( lhs, rhs, detail::mul_t::get( ) );
			}

			template<typename... Op1, typename... Op2>
			constexpr std::tuple<Op1...> operator/( std::tuple<Op1...> const &lhs,
			                                        std::tuple<Op2...> const &rhs ) {
				return detail::apply_tuple_tuple( lhs, rhs, detail::div_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator/( T const &lhs,
			                                        std::tuple<Op1...> const &rhs ) {
				return detail::apply_tuple_value2( lhs, rhs, detail::div_t::get( ) );
			}

			template<typename... Op1, typename T>
			constexpr std::tuple<Op1...> operator/( std::tuple<Op1...> const &lhs,
			                                        T const &rhs ) {
				return detail::apply_tuple_value( lhs, rhs, detail::div_t::get( ) );
			}
		} // namespace operators

		template<typename... Ts>
		constexpr std::ostream &operator<<( std::ostream &os,
		                                    std::tuple<Ts...> const &t ) {
			os << "{";
			for_each( t, detail::print_t{os} );
			os << " }";
			return os;
		}

		template<typename... Ts>
		constexpr auto min( std::tuple<Ts...> const &a,
		                    std::tuple<Ts...> const &b ) {
			return operators::detail::apply_tuple_tuple(
			  a, b, daw::tuple::detail::min_t::get( ) );
		}

		template<typename... Ts>
		constexpr auto max( std::tuple<Ts...> const &a,
		                    std::tuple<Ts...> const &b ) {
			return operators::detail::apply_tuple_tuple(
			  a, b, daw::tuple::detail::max_t::get( ) );
		}

		namespace apply_at_details {
			template<typename Func>
			constexpr void apply_func_impl( Func &&, ... ) noexcept {}

			template<typename Func, typename Arg>
			constexpr void apply_func_impl( Func &&func, Arg &&arg ) noexcept(
			  noexcept( func( std::forward<Arg>( arg ) ) ) ) {
				func( std::forward<Arg>( arg ) );
			}

			template<typename Func>
			struct call_func {
				Func func;

				template<typename Arg, std::enable_if_t<traits::is_callable_v<Func, Arg>,
				                                        std::nullptr_t> = nullptr>
				constexpr void
				operator( )( Arg &&value ) noexcept( noexcept( func( value ) ) ) {
					func( std::forward<Arg>( value ) );
				}

				constexpr void operator( )( ... ) noexcept {}
			};

			template<typename Func>
			constexpr call_func<Func> make_call_func( Func &&func ) noexcept {
				return call_func<Func>{std::forward<Func>( func )};
			}

			template<size_t N, typename Func, typename Arg>
			constexpr void apply_func( size_t idx, Func func, Arg &&arg ) noexcept(
			  noexcept( func( std::forward<Arg>( arg ) ) ) ) {
				if( N == idx ) {
					apply_func_impl( std::move( func ), std::forward<Arg>( arg ) );
				}
			}

			template<typename Tuple, typename Func, size_t... Is>
			constexpr void apply_at_impl( size_t idx, Tuple &&tp, Func &&func,
			                              std::index_sequence<Is...> ) {
				using expander = int[];
				(void)expander{0, ( apply_func<Is>( idx, std::forward<Func>( func ),
				                                    std::get<Is>( tp ) ),
				                    0 )...};
			}

			template<typename... Args, typename Func>
			constexpr void apply_at( size_t idx, std::tuple<Args...> &&tp,
			                         Func &&func ) {
				apply_at_impl( idx, std::move( tp ), std::forward<Func>( func ),
				               std::index_sequence_for<Args...>{} );
			}
		} // namespace apply_at_details
		template<typename... Args, typename Func>
		constexpr void apply_at( size_t idx, std::tuple<Args...> const &tp,
		                         Func &&func ) {
			apply_at_details::apply_at_impl(
			  idx, tp, apply_at_details::make_call_func( std::forward<Func>( func ) ),
			  std::index_sequence_for<Args...>{} );
		}
	} // namespace tuple

	namespace detail {
		template<class T, class Tuple, std::size_t... I>
		constexpr T make_from_tuple_impl2( Tuple &&t, std::index_sequence<I...>, std::true_type ) {
			return T{std::get<I>( std::forward<Tuple>( t ) )...};
		}

		template<class T, class Tuple, std::size_t... I>
		constexpr T make_from_tuple_impl2( Tuple &&t, std::index_sequence<I...>, std::false_type ) {
			return T(std::get<I>( std::forward<Tuple>( t ) )...);
		}
	} // namespace detail

	template<class T, class Tuple>
	constexpr T make_from_tuple2( Tuple &&t ) {
		return detail::make_from_tuple_impl2<T>(
		  std::forward<Tuple>( t ),
		  std::make_index_sequence<
		    std::tuple_size_v<std::remove_reference_t<Tuple>>>{}, std::is_aggregate<T>{} );
	}
} // namespace daw

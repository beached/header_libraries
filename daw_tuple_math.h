// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <tuple>
#include <utility>

namespace daw {
	namespace tuple {
			namespace detail {
				template<typename T>
				void do_nothing( T const & ){ }

				template<int... Is>
				struct seq { };

				template<int N, int... Is>
				struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

				template<int... Is>
				struct gen_seq<0, Is...> : seq<Is...> { };

				template<typename T1, typename F, int... Is>
				constexpr void for_each( T1 && t1, F f, seq<Is...>) {
					auto l = { (f( std::get<Is>(t1) ), 0)... };
					do_nothing( l );
				}

				template<typename... Ts, typename F>
				constexpr void for_each( std::tuple<Ts...> const & t1, F f ) {
					for_each( t1, f, gen_seq<sizeof...(Ts)>());
				}
					
			}	// namespace detail
		namespace operators {
			namespace detail {
				template<typename T>
				void do_nothing( T const & ){ }

				template<int... Is>
					struct seq { };

				template<int N, int... Is>
					struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

				template<int... Is>
					struct gen_seq<0, Is...> : seq<Is...> { };

				template<typename Result, typename T1, typename T2, typename F, int... Is>
					constexpr void apply_tuple( Result && result, T1 && op1, T2 && op2, F f, seq<Is...>) {
						auto l = { (f( std::get<Is>(result), std::get<Is>(op1), std::get<Is>(op2) ), 0)... };
						do_nothing( l );
					}

				template<typename Result, typename T1, typename T, typename F, int... Is>
					constexpr void apply_value( Result && result, T1 && op1, T && op2, F f, seq<Is...>) {
						auto l = { (f( std::get<Is>(result), std::get<Is>(op1), op2 ), 0)... };
						do_nothing( l );
					}

				template<typename Result, typename T1, typename T, typename F, int... Is>
					constexpr void apply_value2( Result && result, T1 && op1, T && op2, F f, seq<Is...>) {
						auto l = { (f( std::get<Is>(result), op1, std::get<Is>(op2) ), 0)... };
						do_nothing( l );
					}

				template<typename... Ts, typename F>
					constexpr std::tuple<Ts...> apply_tuple_tuple( std::tuple<Ts...> const & op1, std::tuple<Ts...> const & op2, F f ) {
						std::tuple<Ts...> result;
						apply_tuple(result, op1, op2, f, gen_seq<sizeof...(Ts)>());
						return result;
					}

				template<typename... Ts, typename T, typename F>
					constexpr std::tuple<Ts...> apply_tuple_value2( T const & op1, std::tuple<Ts...> const & op2, F f ) {
						std::tuple<Ts...> result;
						apply_value2(result, op1, op2, f, gen_seq<sizeof...(Ts)>());
						return result;
					}

				template<typename... Ts, typename T, typename F>
					constexpr std::tuple<Ts...> apply_tuple_value( std::tuple<Ts...> const & op1, T const & op2, F f ) {
						std::tuple<Ts...> result;
						apply_value(result, op1, op2, f, gen_seq<sizeof...(Ts)>());
						return result;
					}
			}	// namespace detail

			template<typename... Op1, typename... Op2>
				std::tuple<Op1...> operator+( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
					return detail::apply_tuple_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l+r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator+( T const & lhs, std::tuple<Op1...> const & rhs ) {
					return detail::apply_tuple_value2( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l+r;
							} );
				}		


			template<typename... Op1, typename T>
				std::tuple<Op1...> operator+( std::tuple<Op1...> const & lhs, T const & rhs ) {
					return detail::apply_tuple_value( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l+r;
							} );
				}		

			template<typename... Op1, typename... Op2>
				std::tuple<Op1...> operator-( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
					return detail::apply_tuple_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l-r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator-( T const & lhs, std::tuple<Op1...> const & rhs ) {
					return detail::apply_tuple_value2( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l-r;
							} );
				}		


			template<typename... Op1, typename T>
				std::tuple<Op1...> operator-( std::tuple<Op1...> const & lhs, T const & rhs ) {
					return detail::apply_tuple_value( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l-r;
							} );
				}		

			template<typename... Op1, typename... Op2>
				std::tuple<Op1...> operator*( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
					return detail::apply_tuple_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l*r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator*( std::tuple<Op1...> const & lhs, T const & rhs ) {
					return detail::apply_tuple_value( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l*r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator*( T const & lhs, std::tuple<Op1...> const & rhs ) {
					return detail::apply_tuple_value2( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l*r;
							} );
				}		

			template<typename... Op1, typename... Op2>
				std::tuple<Op1...> operator/( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
					return detail::apply_tuple_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l/r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator/( std::tuple<Op1...> const & lhs, T const & rhs ) {
					return detail::apply_tuple_value( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l/r;
							} );
				}

			template<typename... Op1, typename T>
				std::tuple<Op1...> operator/( T const & lhs, std::tuple<Op1...> const & rhs ) {
					return detail::apply_tuple_value2( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
							result = l/r;
							} );
				}		


		}	// namespace operators

		template<typename... Ts>
		constexpr std::ostream & operator<<( std::ostream & os, std::tuple<Ts...> const & t ) {
			os << "{";
			bool is_first = true;
			detail::for_each( t, [&is_first, &os]( auto const & v ) {
				if( is_first ) {
					os << " " << v;
					is_first = false;
				} else {
					os << ", " << v;
				}
			} );	
			os << " }";
			return os;
		}

	}	// namespace tuple
}    // namespace daw


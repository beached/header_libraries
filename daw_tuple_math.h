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
			template<int... Is>
			struct seq { };

			template<int N, int... Is>
			struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

			template<int... Is>
			struct gen_seq<0, Is...> : seq<Is...> { };

			template<typename Result, typename T1, typename T2, typename F, int... Is>
			void for_each( Result && result, T1 && op1, T2 && op2, F f, seq<Is...>) {
				auto l = { (f( std::get<Is>(result), std::get<Is>(op1), std::get<Is>(op2) ), 0)... };
			}

			template<typename Result, typename T1, typename T, typename F, int... Is>
			void apply( Result && result, T1 && op1, T && op2, F f, seq<Is...>) {
				auto l = { (f( std::get<Is>(result), std::get<Is>(op1), op2 ), 0)... };
			}

			template<typename... Ts, typename F>
			std::tuple<Ts...> apply_in_tuple( std::tuple<Ts...> const & op1, std::tuple<Ts...> const & op2, F f ) {
				std::tuple<Ts...> result;
				apply(result, op1, op2, f, detail::gen_seq<sizeof...(Ts)>());
				return result;
			}

			template<typename... Ts, typename T, typename F>
			std::tuple<Ts...> apply_in_tuple( std::tuple<Ts...> const & op1, T const & op2, F f ) {
				std::tuple<Ts...> result;
				apply(result, op1, op2, f, detail::gen_seq<sizeof...(Ts)>());
				return result;
			}

		}	// namespace detail

		template<typename... Op1, typename... Op2>
		auto operator+( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l+r;
			} );
		}

		template<typename... Op1, typename T>
		auto operator+( std::tuple<Op1...> const & lhs, T const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l+r;
			} );
		}		
		
		template<typename... Op1, typename... Op2>
		auto operator-( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l-r;
			} );
		}

		template<typename... Op1, typename T>
		auto operator-( std::tuple<Op1...> const & lhs, T const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l-r;
			} );
		}		
		
		template<typename... Op1, typename... Op2>
		auto operator*( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l*r;
			} );
		}

		template<typename... Op1, typename T>
		auto operator*( std::tuple<Op1...> const & lhs, T const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l*r;
			} );
		}

	template<typename... Op1, typename... Op2>
		auto operator/( std::tuple<Op1...> const & lhs, std::tuple<Op2...> const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l/r;
			} );
		}

		template<typename... Op1, typename T>
		auto operator/( std::tuple<Op1...> const & lhs, T const & rhs ) {
			return detail::apply_in_tuple( lhs, rhs, []( auto & result, auto const & l, auto const & r ) {
				result = l/r;
			} );
		}

	}	// namespace tuple
}    // namespace daw


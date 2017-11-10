// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "daw_function.h"
#include "daw_parser_addons.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		struct parser_exception {};
		struct invalid_input_exception : parser_exception {};
		struct empty_input_exception : invalid_input_exception {};

		namespace impl {
			template<size_t N, typename... Args, std::enable_if_t<( N == 0 ), std::nullptr_t> = nullptr>
			constexpr void set_value_from_stream( std::tuple<Args...> &, std::istream & ) noexcept {}

			template<size_t N, typename... Args, std::enable_if_t<( N > 0 ), std::nullptr_t> = nullptr>
			void set_value_from_stream( std::tuple<Args...> &tp, std::istream &io ) {
				io >> std::get<sizeof...( Args ) - N>( tp );
				set_value_from_stream<N - 1>( tp, io );
			}
		} // namespace impl
		template<typename... Args>
		std::tuple<Args...> parse_to( std::istream &io ) {
			std::tuple<Args...> result;
			impl::set_value_from_stream<sizeof...( Args )>( result, io );
			return result;
		}

		namespace default_parsers {
			template<typename T, std::enable_if_t<is_same_v<T, char>, std::nullptr_t> = nullptr>
			constexpr char parser( daw::string_view str ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return str.front( );
			}

			template<typename T,
			         std::enable_if_t<!is_same_v<T, char> && is_integral_v<T> && is_signed_v<T>, std::nullptr_t> = nullptr>
			constexpr T parser( daw::string_view str ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				T result{};
				daw::parser::parse_int( str.cbegin( ), str.cend( ), result );
				return result;
			}

			template<typename T, std::enable_if_t<is_integral_v<T> && is_unsigned_v<T>, std::nullptr_t> = nullptr>
			constexpr T parser( daw::string_view str ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				T result{};
				daw::parser::parse_unsigned_int( str.cbegin( ), str.cend( ), result );
				return result;
			}

			template<typename T, std::enable_if_t<is_same_v<T, std::string>, std::nullptr_t> = nullptr>
			std::string parser( daw::string_view str ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				if( str.size( ) < 2 ) {
					throw invalid_input_exception{};
				}
				if( str.front( ) != '"' && str.back( ) != '"' ) {
					throw invalid_input_exception{};
				}
				str = str.substr( 1, str.size( ) - 2 );
				std::string result{};
				std::copy( str.cbegin( ), str.cend( ), std::back_inserter( result ) );
				return result;
			}

			template<typename T, std::enable_if_t<is_same_v<T, daw::string_view>, std::nullptr_t> = nullptr>
			constexpr daw::string_view parser( daw::string_view str ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				if( str.size( ) < 2 ) {
					throw invalid_input_exception{};
				}
				if( str.front( ) != '"' && str.back( ) != '"' ) {
					throw invalid_input_exception{};
				}
				return str.substr( 1, str.size( ) - 2 );
			}

		} // namespace default_parsers

		namespace impl {
			template<size_t N, std::enable_if_t<( N == 0 ), std::nullptr_t> = nullptr, typename... Args, typename Splitter>
			constexpr void set_value_from_string_view( std::tuple<Args...> &, daw::string_view, Splitter ) noexcept {}

			template<size_t N, std::enable_if_t<( N > 0 ), std::nullptr_t> = nullptr, typename... Args, typename Splitter>
			constexpr void set_value_from_string_view( std::tuple<Args...> &tp, daw::string_view str, Splitter splitter ) {
				static_assert( N <= sizeof...( Args ), "Invalud value of N" );
				auto const end_pos = splitter( str );
				if( N > 1 && end_pos.first == str.npos ) {
					throw invalid_input_exception{};
				}
				using pos_t = std::integral_constant<size_t, sizeof...( Args ) - N>;
				using value_t = std::decay_t<decltype( std::get<pos_t::value>( tp ) )>;

				std::get<pos_t::value>( tp ) = daw::parser::default_parsers::parser<value_t>( str.substr( 0, end_pos.first ) );
				str.remove_prefix( end_pos.last );
				daw::parser::impl::set_value_from_string_view<N - 1>( tp, std::move( str ), std::move( splitter ) );
			}
		} // namespace impl
		class default_splitter {
			daw::string_view m_delemiter;

		public:
			constexpr default_splitter( daw::string_view delemiter ) : m_delemiter{std::move( delemiter )} {}
			constexpr default_splitter( ) noexcept = delete;
			~default_splitter( ) noexcept = default;
			constexpr default_splitter( default_splitter const & ) noexcept = default;
			constexpr default_splitter( default_splitter && ) noexcept = default;
			constexpr default_splitter &operator=( default_splitter const & ) noexcept = default;
			constexpr default_splitter &operator=( default_splitter && ) noexcept = default;

			constexpr auto operator( )( daw::string_view str ) const noexcept {
				struct result_t {
					typename daw::string_view::size_type first;
					typename daw::string_view::size_type last;
				};
				auto const pos = str.find( m_delemiter );
				if( pos < str.size( ) ) {
					return result_t{pos, pos + m_delemiter.size( )};
				}
				return result_t{str.npos, str.npos};
			}
		};
		template<typename... Args, typename Splitter,
		         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
		constexpr std::tuple<Args...> parse_to( daw::string_view str, Splitter splitter ) {
			static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );
			std::tuple<Args...> result;
			impl::set_value_from_string_view<sizeof...( Args )>( result, std::move( str ), std::move( splitter ) );
			return result;
		}

		template<typename... Args>
		constexpr std::tuple<Args...> parse_to( daw::string_view str, daw::string_view delemiter ) {
			std::tuple<Args...> result;
			impl::set_value_from_string_view<sizeof...( Args )>( result, std::move( str ),
			                                                     default_splitter{std::move( delemiter )} );
			return result;
		}

		template<typename... Args>
		constexpr std::tuple<Args...> parse_to( daw::string_view str ) {
			return parse_to( std::move( str ), daw::string_view{" "} );
		}
	} // namespace parser

	namespace impl {
		template<typename T>
		struct make_a {
			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&... args ) const {
				return T{std::forward<Args>( args )...};
			}
		};
	} // namespace impl

	template<typename Destination, typename... ExpectedArgs, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) construct_from( daw::string_view str, Splitter splitter ) {
		static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );

		return daw::apply( impl::make_a<Destination>{},
		                   parser::parse_to<ExpectedArgs...>( std::move( str ), std::move( splitter ) ) );
	}

	template<typename Destination, typename... ExpectedArgs>
	constexpr decltype( auto ) construct_from( daw::string_view str, daw::string_view delemiter ) {
		return construct_from<Destination, ExpectedArgs...>( std::move( str ),
		                                                     parser::default_splitter{std::move( delemiter )} );
	}

	namespace impl {
		template<typename... Args, typename Callable, typename Splitter>
		constexpr decltype( auto ) apply_string_impl( std::tuple<Args...>, Callable callable, daw::string_view str,
		                                              Splitter splitter ) {
			return daw::apply( std::move( callable ), parser::parse_to<Args...>( std::move( str ), std::move( splitter ) ) );
		}
	} // namespace impl
	template<typename Callable, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string( Callable callable, daw::string_view str, Splitter splitter ) {
		static_assert( is_callable_v<Splitter, daw::string_view>, "Splitter is not a callable type" );
		using ftraits = typename daw::function_traits<decltype( callable )>::args_tuple;
		return impl::apply_string_impl( ftraits{}, std::move( callable ), std::move( str ), std::move( splitter ) );
	}

	template<typename Callable>
	constexpr decltype( auto ) apply_string( Callable callable, daw::string_view str, daw::string_view delemiter ) {
		return apply_string<Callable>( std::move( callable ), std::move( str ),
		                               parser::default_splitter{std::move( delemiter )} );
	}
	namespace impl {
		template<size_t CallableArgsArity, size_t ParsedArgsArity>
		class ArityCheckEqual {
			static_assert( CallableArgsArity == ParsedArgsArity, "Callable args arity does not match number of arguments" );
		};
	} // namespace impl

	template<typename... Args, typename Callable, typename Splitter,
	         std::enable_if_t<!is_convertible_v<Splitter, daw::string_view>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string2( Callable callable, daw::string_view str, Splitter splitter ) {
		static_cast<void>( impl::ArityCheckEqual<daw::function_traits<Callable>::arity, sizeof...(Args)>{} );
		return daw::apply( std::move( callable ), parser::parse_to<Args...>( std::move( str ), std::move( splitter ) ) );
	}

	template<typename... Args, typename Callable>
	constexpr decltype( auto ) apply_string2( Callable callable, daw::string_view str, daw::string_view delemiter ) {
		return apply_string2<Args...>( std::move( callable ), std::move( str ),
		                                         parser::default_splitter{std::move( delemiter )} );
	}
} // namespace daw


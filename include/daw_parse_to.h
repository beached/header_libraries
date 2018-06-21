// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <iterator>
#include <string>
#include <tuple>
#include <utility>

#include "daw_function.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		struct parser_exception {};
		struct invalid_input_exception : parser_exception {};
		struct empty_input_exception : invalid_input_exception {};
		struct numeric_overflow_exception : invalid_input_exception {};

		template<typename T>
		class not_dc {
			struct not_avail {};
			union value_t {
				not_avail no_value;
				T has_value;
				constexpr value_t( ) noexcept
				  : no_value{} {}
				template<typename Arg, typename... Args>
				constexpr value_t( Arg &&arg, Args &&... args )
				  : has_value( std::forward<Arg>( arg ),
				               std::forward<Args>( args )... ) {}
			} value;
			bool has_value;

		public:
			constexpr not_dc( ) noexcept
			  : value( )
			  , has_value( false ) {}

			template<typename Arg, typename... Args>
			constexpr not_dc( Arg &&arg, Args &&... args )
			  : value( std::forward<Arg>( arg ), std::forward<Args>( args )... )
			  , has_value( true ) {}

			constexpr operator T &( ) {
				if( !has_value ) {
					throw std::exception( );
				}
				return value.has_value;
			}

			constexpr operator T const &( ) const {
				if( !has_value ) {
					throw std::exception( );
				}
				return value.has_value;
			}
		};

		namespace converters {
			constexpr char parse_to_value( daw::string_view str, char ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return str.front( );
			}

			namespace helpers {
				template<typename Result>
				constexpr Result parse_int( daw::string_view &str ) {
					intmax_t count = std::numeric_limits<Result>::digits10;
					Result result = 0;
					bool is_neg = false;
					if( '-' == str.front( ) ) {
						if( !std::numeric_limits<Result>::is_signed ) {
							throw invalid_input_exception{};
						}
						is_neg = true;
						str.remove_prefix( );
					}
					while( count > 0 && !str.empty( ) && is_number( str.front( ) ) ) {
						result *= static_cast<Result>( 10 );
						result += static_cast<Result>( str.front( ) - '0' );
						--count;
						str.remove_prefix( );
					}
					if( !str.empty( ) ) {
						throw numeric_overflow_exception{};
					}
					if( is_neg ) {
						result *= static_cast<Result>( -1 );
					}
					return result;
				}

				template<typename Result>
				constexpr Result parse_unsigned_int( daw::string_view &str ) {
					if( '-' == str.front( ) ) {
						throw invalid_input_exception{};
					}
					return parse_int<Result>( str );
				}

			} // namespace helpers

			template<typename T,
			         std::enable_if_t<(!is_same_v<T, char> && is_integral_v<T> &&
			                           is_signed_v<T> && !is_enum_v<T>),
			                          std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, T ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return helpers::parse_int<T>( str );
			}

			template<typename T,
			         std::enable_if_t<is_integral_v<T> && is_unsigned_v<T>,
			                          std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, T ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return helpers::parse_unsigned_int<T>( str );
			}

			namespace impl {
				constexpr bool is_quote( char const last_char, char const c ) noexcept {
					return last_char != '\\' && c == '"';
				}
			} // namespace impl

			struct unquoted_string {};
			struct unquoted_string_view {};

			constexpr daw::string_view parse_to_value( daw::string_view str,
			                                           unquoted_string_view ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				return str;
			}

			constexpr daw::string_view parse_to_value( daw::string_view str,
			                                           daw::string_view ) {
				if( str.empty( ) ) {
					throw empty_input_exception{};
				}
				if( str.size( ) < 2 ) {
					struct input_too_small_exception {};
					throw input_too_small_exception{};
				}
				if( str.front( ) != '"' ) {
					struct missing_expected_quotes_exception {};
					throw missing_expected_quotes_exception{};
				}
				str.remove_prefix( );
				auto const first = str.cbegin( );
				char last_char = str.pop_front( );
				while( !str.empty( ) && !impl::is_quote( last_char, str.front( ) ) ) {
					last_char = str.pop_front( );
				}
				if( str.front( ) != '"' ) {
					struct missing_expected_quotes_exception {};
					throw missing_expected_quotes_exception{};
				}
				return daw::make_string_view_it( first, str.cbegin( ) );
			}

			inline std::string parse_to_value( daw::string_view str, std::string ) {
				return parse_to_value( str, daw::string_view{} ).to_string( );
			}

			inline std::string parse_to_value( daw::string_view str,
			                                   unquoted_string ) {
				return parse_to_value( str, unquoted_string_view{} ).to_string( );
			}

			inline float parse_to_value( daw::string_view str, float ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtof( s.c_str( ), end );
			}

			inline double parse_to_value( daw::string_view str, double ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtod( s.c_str( ), end );
			}

			inline long double parse_to_value( daw::string_view str, long double ) {
				auto const s = str.to_string( );
				char **end = nullptr;
				return strtold( s.c_str( ), end );
			}

			template<typename EnumType,
			         std::enable_if_t<is_enum_v<EnumType>, std::nullptr_t> = nullptr>
			constexpr decltype( auto )
			parse_to_value( daw::string_view str, EnumType ) noexcept(
			  noexcept( parse_enum_value( str, std::declval<EnumType>( ) ) ) ) {

				return parse_enum_value( str, EnumType{} );
			}
		} // namespace converters

		namespace impl {
			template<size_t N, typename... Args,
			         std::enable_if_t<( N == 0 ), std::nullptr_t> = nullptr,
			         typename Tuple, typename Splitter>
			constexpr void set_value_from_string_view( Tuple &, daw::string_view,
			                                           Splitter && ) {}

			template<size_t N, typename... Args, typename Tuple, typename Splitter,
			         std::enable_if_t<( N > 0 && N <= sizeof...( Args ) ),
			                          std::nullptr_t> = nullptr>
			constexpr void set_value_from_string_view( Tuple &tp,
			                                           daw::string_view str,
			                                           Splitter &&splitter ) {

				auto const end_pos = splitter( str );
				if( N > 1 && end_pos.first == str.npos ) {
					throw invalid_input_exception( );
				}
				using namespace ::daw::parser::converters;

				constexpr auto const pos_v = sizeof...( Args ) - N;

				using value_t = daw::pack_type_t<pos_v, Args...>;

				std::get<pos_v>( tp ) =
				  parse_to_value( str.substr( 0, end_pos.first ), value_t{} );

				str.remove_prefix( end_pos.last );
				daw::parser::impl::set_value_from_string_view<N - 1, Args...>(
				  tp, str, std::forward<Splitter>( splitter ) );
			}
		} // namespace impl

		/// @brief Split on supplied delemiter string.
		struct default_splitter {
			daw::string_view m_delemiter;

			constexpr auto operator( )( daw::string_view str ) const noexcept {
				using sv_size_t = typename daw::string_view::size_type;
				struct result_t {
					sv_size_t first;
					sv_size_t last;
				};
				auto const pos = str.find( m_delemiter );
				if( pos < str.size( ) ) {
					return result_t{pos, pos + m_delemiter.size( )};
				}
				return result_t{str.npos, str.npos};
			}
		};

		/// @brief Splits a string on any unicode whitespace characters
		/// @tparam skip_multiple If multiple whitespace characters are seen in a
		/// row, skip them all
		template<bool skip_multiple>
		struct basic_whitespace_splitter {
			constexpr auto operator( )( daw::string_view str ) const {
				using sv_size_t = typename daw::string_view::size_type;
				struct result_t {
					sv_size_t first;
					sv_size_t last;
				};

				sv_size_t n = 0;
				size_t const sz = str.size( );

				while( n < sz && not_unicode_whitespace( str[n] ) ) {
					++n;
				}
				if( n == sz ) {
					return result_t{str.npos, str.npos};
				}
				if( !skip_multiple ) {
					return result_t{n, n + 1};
				}
				auto const f = n;
				while( n < sz && is_unicode_whitespace( str[n] ) ) {
					++n;
				}
				return result_t{f, n};
			}
		};

		/// @brief Whitespace splitter that splits on each encounter
		using whitespace_splitter = basic_whitespace_splitter<true>;

		/// @brief Whitespace splitter that will merge whitespace as 1 delemiter
		using single_whitespace_splitter = basic_whitespace_splitter<false>;

		namespace impl {
			template<typename T, std::enable_if_t<is_default_constructible_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr decltype( auto ) parse_result_of_test( ) noexcept {
				using namespace ::daw::parser::converters;
				return parse_to_value( daw::string_view( ), T( ) );
			}

			template<typename T, std::enable_if_t<!is_default_constructible_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr decltype( auto ) parse_result_of_test( ) noexcept {
				using namespace ::daw::parser::converters;
				return parse_to_value( daw::string_view( ), daw::parser::not_dc<T>( ) );
			}

			template<typename T>
			using parse_result_of_t =
			  std::decay_t<decltype( parse_result_of_test<T>( ) )>;
		} // namespace impl

		/// @brief Attempts to parse a string to the values types specified
		/// @tparam Args Result types of values encoded as strings
		/// @tparam Splitter Callable splitter that returns the next position of a
		/// value
		/// @param str String containing encoded values
		/// @param splitter Function to split string into arguments
		/// @return A tuple of values of the types specified in Args
		template<typename... Args, typename Splitter,
		         std::enable_if_t<is_callable_v<Splitter, daw::string_view>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) parse_to( daw::string_view str,
		                                     Splitter &&splitter ) {

			auto result = std::tuple<impl::parse_result_of_t<Args>...>( );
			impl::set_value_from_string_view<sizeof...( Args ), Args...>(
			  result, str, std::forward<Splitter>( splitter ) );
			return result;
		}

		/// @brief Attempts to parse a string to the values types specified
		/// @tparam Args Result types of values encoded as strings
		/// @param str String containing encoded values
		/// @param delemiter split what string arguments on
		/// @return A tuple of values of the types specified in Args
		template<typename... Args>
		constexpr decltype( auto ) parse_to( daw::string_view str,
		                                     daw::string_view delemiter ) {
			return parse_to<Args...>( str, default_splitter{delemiter} );
		}

		/// @brief Attempts to parse a string to the values types specified in args
		/// and sepearated by a " "
		/// @tparam Args Result types of values encoded as strings
		/// @param str String containing encoded values
		/// @return A tuple of values of the types specified in Args
		template<typename... Args>
		constexpr decltype( auto ) parse_to( daw::string_view str ) {
			return parse_to<Args...>( str, daw::string_view( " " ) );
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

	/// @brief Contructs an object from the arguments specified in the string.
	/// @tparam Destination The type of object to construct
	/// @tparam ExpectedArgs The types of values to parse out of the string
	/// @tparam Splitter Callable splitter that returns the next position of a
	/// value
	/// @param str String containing encoded values
	/// @param splitter split what string arguments on
	/// @return A constructed Destination
	template<typename Destination, typename... ExpectedArgs, typename Splitter,
	         std::enable_if_t<is_callable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) construct_from( daw::string_view str,
	                                           Splitter &&splitter ) {

		return daw::apply( impl::make_a<Destination>{},
		                   parser::parse_to<ExpectedArgs...>(
		                     str, std::forward<Splitter>( splitter ) ) );
	}

	/// @brief Contructs an object from the arguments specified in the string.
	/// @tparam Destination The type of object to construct
	/// @tparam ExpectedArgs The types of values to parse out of the string
	/// @param str String containing encoded values
	/// @param delemiter that str is split on
	/// @return A constructed Destination
	template<typename Destination, typename... ExpectedArgs>
	constexpr decltype( auto ) construct_from( daw::string_view str,
	                                           daw::string_view delemiter ) {

		return construct_from<Destination, ExpectedArgs...>(
		  str, parser::default_splitter{delemiter} );
	}

	/// @brief Contructs an object from the arguments specified in the string
	/// using a default delemiter of " ".
	/// @tparam Destination The type of object to construct
	/// @tparam ExpectedArg The type of value to parse out of the string
	/// @param str String containing encoded values
	/// @return A constructed Destination
	template<typename Destination, typename ExpectedArg>
	constexpr decltype( auto ) construct_from( daw::string_view str ) {
		return construct_from<Destination, ExpectedArg>(
		  str, parser::default_splitter{" "} );
	}

	namespace impl {
		template<typename... Args, typename Callable, typename Splitter>
		constexpr decltype( auto )
		apply_string_impl( std::tuple<Args...>, Callable &&callable,
		                   daw::string_view str, Splitter &&splitter ) {
			return daw::apply(
			  std::forward<Callable>( callable ),
			  parser::parse_to<Args...>( str, std::forward<Splitter>( splitter ) ) );
		}
	} // namespace impl

	/// @brief Apply the reified string as the types deducted from the Callable to
	/// the Callable.  This may have unexpected results if strings are ot
	/// controlled
	/// @tparam Callable Can be called with operator( )
	/// @tparam Splitter Callable splitter that returns the next position of a
	/// value
	/// @param callable a function that takes deducted arguments
	/// @param str String containing encoded values
	/// @param splitter Function to split string into arguments
	/// @return The result of callable
	template<typename Callable, typename Splitter,
	         std::enable_if_t<is_callable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string( Callable &&callable,
	                                         daw::string_view str,
	                                         Splitter &&splitter ) {
		using ftraits =
		  typename daw::function_info<std::decay_t<Callable>>::decayed_args_tuple;
		return impl::apply_string_impl( ftraits{},
		                                std::forward<Callable>( callable ), str,
		                                std::forward<Splitter>( splitter ) );
	}

	/// @brief Apply the reified string as the types deducted from the Callable to
	/// the Callable.  This may have unexpected results if strings are ot
	/// controlled
	/// @tparam Callable Can be called with operator( )
	/// @param callable a function that takes deducted arguments
	/// @param str String containing encoded values
	/// @param delemiter split what string arguments on
	/// @return The result of callable
	template<typename Callable>
	constexpr decltype( auto ) apply_string( Callable &&callable,
	                                         daw::string_view str,
	                                         daw::string_view delemiter ) {
		return apply_string<Callable>( std::forward<Callable>( callable ), str,
		                               parser::default_splitter{delemiter} );
	}
	namespace impl {
		template<size_t CallableArgsArity, size_t ParsedArgsArity>
		class ArityCheckEqual {
			static_assert( CallableArgsArity == ParsedArgsArity,
			               "Callable args arity does not match number of arguments" );
		};
	} // namespace impl

	/// @brief Apply the reified string as the types specified as Args... to the
	/// Callable
	/// @tparam Args Types of expected data to find in string
	/// @tparam Callable Callable function type that will accept the args in
	/// string
	/// @tparam Splitter A predicate that will return true on the string parts to
	/// split on
	/// @param callable Function to apply arguments to
	/// @param str String data with string encoded arguments
	/// @param splitter Function to split string into arguments
	/// @return result of callable
	template<typename... Args, typename Callable, typename Splitter,
	         std::enable_if_t<is_callable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string2( Callable &&callable,
	                                          daw::string_view str,
	                                          Splitter &&splitter ) {
		static_assert( is_callable_v<Callable, Args...>,
		               "Callable must accept Args..." );
		return daw::apply(
		  std::forward<Callable>( callable ),
		  parser::parse_to<Args...>( str, std::forward<Splitter>( splitter ) ) );
	}

	/// @brief Apply the reified string as the types specified as Args... to the
	/// Callable
	/// @tparam Args Types of expected data to find in string
	/// @tparam Callable Callable function type that will accept the args in
	/// string
	/// @param callable Function to apply arguments to
	/// @param str String data with string encoded arguments
	/// @param delemiter split what string arguments on
	/// @return result of callable
	template<typename... Args, typename Callable,
	         std::enable_if_t<is_callable_v<Callable, Args...>, std::nullptr_t> =
	           nullptr>
	constexpr decltype( auto ) apply_string2( Callable &&callable,
	                                          daw::string_view str,
	                                          daw::string_view delemiter ) {

		return apply_string2<Args...>( std::forward<Callable>( callable ), str,
		                               parser::default_splitter{delemiter} );
	}

	/// @brief Apply the reified string as the types specified as Arg to the
	/// Callable
	/// @tparam Arg Type of expected data to find in string
	/// @tparam Callable Callable function type that will accept the arg in
	/// string
	/// @param callable Function to apply argument to
	/// @param str String data with string encoded argument
	/// @return result of callable
	template<
	  typename Arg, typename Callable,
	  std::enable_if_t<is_callable_v<Callable, Arg>, std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string2( Callable &&callable,
	                                          daw::string_view str ) {

		return apply_string2<Arg>( std::forward<Callable>( callable ), str,
		                           parser::default_splitter{" "} );
	}

	namespace detectors {
		template<typename Stream>
		using has_str = decltype( Stream( ).str( ) );
	}

	/// @brief Extract specified argument types from a stream of character data
	/// @tparam Args Types of expected data to find in string
	/// @tparam Stream Text stream type that does not have a str( ) method
	/// @tparam Splitter A predicate that will return true on the string parts to
	/// split on
	/// @param stream text stream to extract values from
	/// @param splitter Function to split string into arguments
	/// @return A tuple of values of the types specified in Args
	template<typename... Args, typename Stream, typename Splitter,
	         std::enable_if_t<(!is_detected_v<detectors::has_str, Stream> &&
	                           is_callable_v<Splitter, daw::string_view>),
	                          std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&stream, Splitter &&splitter ) {

		auto const str = std::string( std::istreambuf_iterator<char>{stream}, {} );
		return parser::parse_to<Args...>( daw::string_view( str ),
		                                  std::forward<Splitter>( splitter ) );
	}

	/// @brief Extract specified argument types from a stream of character data
	/// @tparam Args Types of expected data to find in string
	/// @tparam Stream Text stream type, like stringstream, that has a str( )
	/// method
	/// @tparam Splitter A predicate that will return true on the string parts to
	/// split on
	/// @param s text stream to extract values from
	/// @param splitter Function to split string into arguments
	/// @return A tuple of values of the types specified in Args
	template<typename... Args, typename Stream, typename Splitter,
	         std::enable_if_t<(is_detected_v<detectors::has_str, Stream> &&
	                           is_callable_v<Splitter, daw::string_view>),
	                          std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&s, Splitter &&splitter ) {

		return parser::parse_to<Args...>( daw::string_view( s.str( ) ),
		                                  std::forward<Splitter>( splitter ) );
	}

	/// @brief Extract specified argument types from a stream of character data
	/// @tparam Args Types of expected data to find in string
	/// @tparam Stream Text stream type
	/// @param s text stream to extract values from
	/// @param delemiter split what string arguments on
	/// @return A tuple of values of the types specified in Args
	template<typename... Args, typename Stream>
	decltype( auto ) values_from_stream( Stream &&s,
	                                     daw::string_view delemiter ) {
		return values_from_stream<Args...>( std::forward<Stream>( s ),
		                                    parser::default_splitter{delemiter} );
	}
} // namespace daw

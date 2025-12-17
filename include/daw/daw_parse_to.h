// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_function.h"
#include "daw_move.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "impl/daw_make_trait.h"

#include <array>
#include <cstddef>
#include <iterator>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace daw {
	namespace parser {
		struct parser_exception {};
		struct invalid_input_exception : parser_exception {};
		struct empty_input_exception : invalid_input_exception {};
		struct numeric_overflow_exception : invalid_input_exception {};

		namespace converters {
			constexpr char parse_to_value( daw::string_view str, tag_t<char> ) {
				daw::exception::precondition_check<empty_input_exception>(
				  !str.empty( ) );
				return str.front( );
			}

			namespace helpers {
				template<typename Result>
				constexpr Result parse_int( daw::string_view &str ) {
					intmax_t count = std::numeric_limits<Result>::digits;
					Result result = 0;
					bool is_neg = false;
					if( '-' == str.front( ) ) {

						daw::exception::precondition_check<invalid_input_exception>(
						  std::numeric_limits<Result>::is_signed );

						is_neg = true;
						str.remove_prefix( );
					}
					while( count > 0 and !str.empty( ) and is_number( str.front( ) ) ) {
						result *= static_cast<Result>( 10 );
						result += static_cast<Result>( str.front( ) - '0' );
						--count;
						str.remove_prefix( );
					}

					daw::exception::precondition_check<numeric_overflow_exception>(
					  str.empty( ) );
					if( is_neg ) {
						result *= static_cast<Result>( -1 );
					}
					return result;
				}

				template<typename Result>
				constexpr Result parse_unsigned_int( daw::string_view &str ) {
					daw::exception::precondition_check<invalid_input_exception>(
					  '-' != str.front( ) );

					return parse_int<Result>( str );
				}

			} // namespace helpers

			template<typename T,
			         std::enable_if_t<
			           all_true_v<not std::is_same_v<T, char>, std::is_integral_v<T>,
			                      std::is_signed_v<T>, not std::is_enum_v<T>>,
			           std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, tag_t<T> ) {
				daw::exception::precondition_check<empty_input_exception>(
				  !str.empty( ) );
				return helpers::parse_int<T>( str );
			}

			template<typename T, std::enable_if_t<all_true_v<std::is_integral_v<T>,
			                                                 std::is_unsigned_v<T>>,
			                                      std::nullptr_t> = nullptr>
			constexpr T parse_to_value( daw::string_view str, tag_t<T> ) {
				daw::exception::precondition_check<empty_input_exception>(
				  not str.empty( ) );

				return helpers::parse_unsigned_int<T>( str );
			}

			namespace impl {
				constexpr bool is_quote( char const last_char, char const c ) noexcept {
					return last_char != '\\' and c == '"';
				}
			} // namespace impl

			struct unquoted_string {};
			struct unquoted_string_view {};

			constexpr daw::string_view parse_to_value( daw::string_view str,
			                                           tag_t<unquoted_string_view> ) {
				daw::exception::precondition_check<empty_input_exception>(
				  !str.empty( ) );

				return str;
			}

			constexpr daw::string_view parse_to_value( daw::string_view str DAW_LIFETIME_BOUND,
			                                           tag_t<daw::string_view> ) {
				daw::exception::precondition_check<empty_input_exception>(
				  not str.empty( ) );

				struct input_too_small_exception {};
				struct missing_expected_quotes_exception {};

				daw::exception::precondition_check<input_too_small_exception>(
				  str.size( ) > 2 );
				daw::exception::precondition_check<missing_expected_quotes_exception>(
				  str.front( ) == '"' );

				str.remove_prefix( );
				auto const first = str.data( );
				char last_char = str.pop_front( );
				while( not str.empty( ) and
				       not impl::is_quote( last_char, str.front( ) ) ) {
					last_char = str.pop_front( );
				}
				daw::exception::precondition_check<missing_expected_quotes_exception>(
				  str.data( ) != nullptr and str.front( ) == '"' );

				return daw::string_view( first, str.data( ) );
			}

			inline std::string parse_to_value( daw::string_view str,
			                                   tag_t<std::string> ) {
				return static_cast<std::string>(
				  parse_to_value( str, tag<daw::string_view> ) );
			}

			inline std::string parse_to_value( daw::string_view str,
			                                   tag_t<unquoted_string> ) {
				return static_cast<std::string>(
				  parse_to_value( str, tag<unquoted_string_view> ) );
			}

			inline float parse_to_value( daw::string_view str, tag_t<float> ) {
				auto const s = static_cast<std::string>( str );
				char **end = nullptr;
				return strtof( s.c_str( ), end );
			}

			inline double parse_to_value( daw::string_view str, tag_t<double> ) {
				auto const s = static_cast<std::string>( str );
				char **end = nullptr;
				return strtod( s.c_str( ), end );
			}

			inline long double parse_to_value( daw::string_view str,
			                                   tag_t<long double> ) {
				auto const s = static_cast<std::string>( str );
				char **end = nullptr;
				return strtold( s.c_str( ), end );
			}
		} // namespace converters

		namespace impl {
			template<size_t N, typename... Args>
			constexpr decltype( auto ) set_value_from_string_view_item(
			  std::array<daw::string_view, sizeof...( Args )> const &positions ) {
				using daw::parser::converters::parse_to_value;
				using result_t = daw::pack_type_t<N, Args...>;
				return parse_to_value( positions[N], tag<result_t> );
			}

			template<typename... Args, size_t... Is>
			constexpr decltype( auto ) set_value_from_string_view(
			  std::array<daw::string_view, sizeof...( Args )> const &positions,
			  std::index_sequence<Is...> ) {
				return std::make_tuple(
				  set_value_from_string_view_item<Is, Args...>( positions )... );
			}

			template<typename... Args>
			constexpr decltype( auto ) set_value_from_string_view(
			  std::array<daw::string_view, sizeof...( Args )> const &positions ) {
				return set_value_from_string_view<Args...>(
				  positions, std::index_sequence_for<Args...>{ } );
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
					return result_t{ pos, pos + m_delemiter.size( ) };
				}
				return result_t{ daw::string_view::npos, daw::string_view::npos };
			}
		};

		/// @brief Splits a string on any unicode whitespace characters
		/// @tparam skip_multiple If multiple whitespace characters are seen in a
		/// row, skip them all
		template<bool skip_multiple>
		struct basic_whitespace_splitter {
			constexpr auto operator( )( daw::string_view str ) const {
				struct result_t {
					typename daw::string_view::size_type first;
					typename daw::string_view::size_type last;
				};

				using sv_size_t = typename daw::string_view::size_type;
				sv_size_t n = 0;
				size_t const sz = str.size( );

				while( n < sz and not_unicode_whitespace( str[n] ) ) {
					++n;
				}
				if( n == sz ) {
					return result_t{ daw::string_view::npos, daw::string_view::npos };
				}
				if( !skip_multiple ) {
					return result_t{ n, n + 1 };
				}
				auto const f = n;
				while( n < sz and is_unicode_whitespace( str[n] ) ) {
					++n;
				}
				return result_t{ f, n };
			}
		};

		/// @brief Whitespace splitter that splits on each encounter
		using whitespace_splitter = basic_whitespace_splitter<true>;

		/// @brief Whitespace splitter that will merge whitespace as 1 delemiter
		using single_whitespace_splitter = basic_whitespace_splitter<false>;

		namespace impl {
			template<typename T>
			constexpr decltype( auto ) parse_result_of_test( ) noexcept {
				using namespace daw::parser::converters;
				return parse_to_value( daw::string_view( ), tag<T> );
			}

			template<typename T>
			using parse_result_of_t =
			  std::decay_t<decltype( parse_result_of_test<T>( ) )>;

			template<size_t N, typename Splitter>
			constexpr auto get_positions( daw::string_view str,
			                              Splitter &&splitter ) {
				std::array<daw::string_view, N> result{ };
				for( auto &item : result ) {
					auto const pos = splitter( str );
					item = str.substr( 0, pos.first );
					str.remove_prefix( pos.last );
				}
				return result;
			}
		} // namespace impl

		/// @brief Attempts to parse a string to the values types specified
		/// @tparam Args Result types of values encoded as strings
		/// @tparam Splitter Callable splitter that returns the next position of a
		/// value
		/// @param str String containing encoded values
		/// @param splitter Function to split string into arguments
		/// @return A tuple of values of the types specified in Args
		template<typename... Args, typename Splitter,
		         std::enable_if_t<std::is_invocable_v<Splitter, daw::string_view>,
		                          std::nullptr_t> = nullptr>
		constexpr decltype( auto ) parse_to( daw::string_view str,
		                                     Splitter &&splitter ) {

			return impl::set_value_from_string_view<Args...>(
			  impl::get_positions<sizeof...( Args )>( str, DAW_FWD( splitter ) ) );
		}

		/// @brief Attempts to parse a string to the values types specified
		/// @tparam Args Result types of values encoded as strings
		/// @param str String containing encoded values
		/// @param delemiter split what string arguments on
		/// @return A tuple of values of the types specified in Args
		template<typename... Args>
		constexpr decltype( auto ) parse_to( daw::string_view str,
		                                     daw::string_view delemiter ) {
			return parse_to<Args...>( str, default_splitter{ delemiter } );
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

	/// @brief Contructs an object from the arguments specified in the string.
	/// @tparam Destination The type of object to construct
	/// @tparam ExpectedArgs The types of values to parse out of the string
	/// @tparam Splitter Callable splitter that returns the next position of a
	/// value
	/// @param str String containing encoded values
	/// @param splitter split what string arguments on
	/// @return A constructed Destination
	template<typename Destination, typename... ExpectedArgs, typename Splitter,
	         std::enable_if_t<std::is_invocable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) construct_from( daw::string_view str,
	                                           Splitter &&splitter ) {

		return std::apply(
		  daw::construct_a<Destination>,
		  parser::parse_to<ExpectedArgs...>( str, DAW_FWD( splitter ) ) );
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
		  str, parser::default_splitter{ delemiter } );
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
		  str, parser::default_splitter{ " " } );
	}

	namespace impl {
		template<typename... Args, typename Callable, typename Splitter>
		constexpr decltype( auto )
		apply_string_impl( std::tuple<Args...>, Callable &&callable,
		                   daw::string_view str, Splitter &&splitter ) {
			return std::apply( DAW_FWD( callable ), parser::parse_to<Args...>(
			                                          str, DAW_FWD( splitter ) ) );
		}
	} // namespace impl

	/// @brief Apply the reified string as the types deducted from the Callable
	/// to the Callable.  This may have unexpected results if strings are ot
	/// controlled
	/// @tparam Callable Can be called with operator( )
	/// @tparam Splitter Callable splitter that returns the next position of a
	/// value
	/// @param callable a function that takes deducted arguments
	/// @param str String containing encoded values
	/// @param splitter Function to split string into arguments
	/// @return The result of callable
	template<typename Callable, typename Splitter,
	         std::enable_if_t<std::is_invocable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string2( Callable &&callable,
	                                          daw::string_view str,
	                                          Splitter &&splitter ) {
		using ftraits =
		  typename daw::function_info<std::decay_t<Callable>>::decayed_args_tuple;
		return impl::apply_string_impl( ftraits{ }, DAW_FWD( callable ), str,
		                                DAW_FWD( splitter ) );
	}

	/// @brief Apply the reified string as the types deducted from the Callable
	/// to the Callable.  This may have unexpected results if strings are ot
	/// controlled
	/// @tparam Callable Can be called with operator( )
	/// @param callable a function that takes deducted arguments
	/// @param str String containing encoded values
	/// @param delemiter split what string arguments on
	/// @return The result of callable
	template<typename Callable>
	constexpr decltype( auto ) apply_string2( Callable &&callable,
	                                          daw::string_view str,
	                                          daw::string_view delemiter ) {
		return apply_string2<Callable>( DAW_FWD( callable ), str,
		                                parser::default_splitter{ delemiter } );
	}

	/// @brief Apply the reified string as the types specified as Args... to the
	/// Callable
	/// @tparam Args Types of expected data to find in string
	/// @tparam Callable Callable function type that will accept the args in
	/// string
	/// @tparam Splitter A predicate that will return true on the string parts
	/// to split on
	/// @param callable Function to apply arguments to
	/// @param str String data with string encoded arguments
	/// @param splitter Function to split string into arguments
	/// @return result of callable
	template<typename... Args, typename Callable, typename Splitter,
	         std::enable_if_t<std::is_invocable_v<Splitter, daw::string_view>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) apply_string( Callable &&callable,
	                                         daw::string_view str,
	                                         Splitter &&splitter ) {
		//		static_assert( std::is_invocable_v<Callable, Args...>,
		//		               "Callable must accept Args..." );
		return std::apply( DAW_FWD( callable ),
		                   parser::parse_to<Args...>( str, DAW_FWD( splitter ) ) );
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
	/*	template<typename... Args, typename Callable,
	           std::enable_if_t<std::is_invocable_v<Callable, Args...>,
	   std::nullptr_t> = nullptr>
	             */
	template<typename... Args, typename Callable>
	constexpr decltype( auto ) apply_string( Callable &&callable,
	                                         daw::string_view str,
	                                         daw::string_view delemiter ) {

		return apply_string<Args...>( DAW_FWD( callable ), str,
		                              parser::default_splitter{ delemiter } );
	}

	/// @brief Apply the reified string as the types specified as Arg to the
	/// Callable
	/// @tparam Arg Type of expected data to find in string
	/// @tparam Callable Callable function type that will accept the arg in
	/// string
	/// @param callable Function to apply argument to
	/// @param str String data with string encoded argument
	/// @return result of callable
	/*
	template<
	  typename Arg, typename Callable,
	  std::enable_if_t<std::is_invocable_v<Callable, Arg>, std::nullptr_t> =
	nullptr>
	  */
	template<typename Arg, typename Callable>
	constexpr decltype( auto ) apply_string( Callable &&callable,
	                                         daw::string_view str ) {

		return apply_string<Arg>( DAW_FWD( callable ), str,
		                          parser::default_splitter{ " " } );
	}

	namespace parse_to_impl {
		DAW_MAKE_REQ_TRAIT( has_str_member_v, std::declval<T>( ).str( ) );
	}

	/// @brief Extract specified argument types from a stream of character data
	/// @tparam Args Types of expected data to find in string
	/// @tparam Stream Text stream type that does not have a str( ) method
	/// @tparam Splitter A predicate that will return true on the string parts
	/// to split on
	/// @param stream text stream to extract values from
	/// @param splitter Function to split string into arguments
	/// @return A tuple of values of the types specified in Args
	template<typename... Args, typename Stream, typename Splitter,
	         std::enable_if_t<(not parse_to_impl::has_str_member_v<Stream> and
	                           std::is_invocable_v<Splitter, daw::string_view>),
	                          std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&stream, Splitter &&splitter ) {

		auto const str =
		  std::string( std::istreambuf_iterator<char>{ stream }, { } );
		return parser::parse_to<Args...>( daw::string_view( str ),
		                                  DAW_FWD( splitter ) );
	}

	/// @brief Extract specified argument types from a stream of character data
	/// @tparam Args Types of expected data to find in string
	/// @tparam Stream Text stream type, like stringstream, that has a str( )
	/// method
	/// @tparam Splitter A predicate that will return true on the string parts
	/// to split on
	/// @param s text stream to extract values from
	/// @param splitter Function to split string into arguments
	/// @return A tuple of values of the types specified in Args
	template<typename... Args, typename Stream, typename Splitter,
	         std::enable_if_t<(parse_to_impl::has_str_member_v<Stream> and
	                           std::is_invocable_v<Splitter, daw::string_view>),
	                          std::nullptr_t> = nullptr>
	decltype( auto ) values_from_stream( Stream &&s, Splitter &&splitter ) {

		return parser::parse_to<Args...>( daw::string_view( s.str( ) ),
		                                  DAW_FWD( splitter ) );
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
		return values_from_stream<Args...>( DAW_FWD( s ),
		                                    parser::default_splitter{ delemiter } );
	}
} // namespace daw

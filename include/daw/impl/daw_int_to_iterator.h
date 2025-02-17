// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_arith_traits.h"
#include "daw/daw_exception.h"
#include "daw/daw_traits.h"
#include "daw/deprecated/daw_bounded_string.h"

#include <limits>

namespace daw {
	namespace impl {
		template<typename T, typename Result = daw::remove_cvref_t<T>>
		constexpr Result num_digits( T value ) noexcept {
			static_assert( std::is_integral_v<Result>,
			               "Result must be an integer type" );
			if( value == 0 ) {
				return static_cast<Result>( 1 );
			}
			Result result = 0;
			while( value != 0 ) {
				value /= 10;
				++result;
			}
			return result;
		}

		template<
		  typename Result, typename Integer,
		  std::enable_if_t<std::is_integral_v<Result>, std::nullptr_t> = nullptr>
		constexpr Result pow10( Integer n ) noexcept {
			uint64_t const vals[10] = { 1ULL,
			                            100ULL,
			                            10000ULL,
			                            1000000ULL,
			                            100000000ULL,
			                            10000000000ULL,
			                            1000000000000ULL,
			                            100000000000000ULL,
			                            10000000000000000ULL,
			                            1000000000000000000ULL };
			if( n % 2 == 0 ) {
				return static_cast<Result>( vals[n / 2] );
			}
			return static_cast<Result>( vals[n / 2] * 10ULL );
		}
	} // namespace impl

	namespace int_string_sizes {
		template<size_t N>
		constexpr size_t get( ) noexcept {
			return 0;
		}

		template<>
		constexpr size_t get<1>( ) noexcept {
			return 5;
		}

		template<>
		constexpr size_t get<2>( ) noexcept {
			return 7;
		}

		template<>
		constexpr size_t get<4>( ) noexcept {
			return 11;
		}

		template<>
		constexpr size_t get<8>( ) noexcept {
			return 21;
		}
	} // namespace int_string_sizes
	namespace min_strings {
		constexpr auto get( char, std::integral_constant<size_t, 1> ) noexcept {
			return daw::basic_bounded_string<char, int_string_sizes::get<1>( )>(
			  "-128" );
		}

		constexpr auto get( wchar_t, std::integral_constant<size_t, 1> ) noexcept {
			return daw::basic_bounded_string<wchar_t, int_string_sizes::get<1>( )>(
			  L"-128" );
		}

		constexpr auto get( char, std::integral_constant<size_t, 2> ) noexcept {
			return daw::basic_bounded_string<char, int_string_sizes::get<2>( )>(
			  "-32768" );
		}

		constexpr auto get( wchar_t, std::integral_constant<size_t, 2> ) noexcept {
			return daw::basic_bounded_string<wchar_t, int_string_sizes::get<2>( )>(
			  L"-32768" );
		}

		constexpr auto get( char, std::integral_constant<size_t, 4> ) noexcept {
			return daw::basic_bounded_string<char, int_string_sizes::get<4>( )>(
			  "-2147483648" );
		}

		constexpr auto get( wchar_t, std::integral_constant<size_t, 4> ) noexcept {
			return daw::basic_bounded_string<wchar_t, int_string_sizes::get<4>( )>(
			  L"-2147483648" );
		}

		constexpr auto get( char, std::integral_constant<size_t, 8> ) noexcept {
			return daw::basic_bounded_string<char, int_string_sizes::get<8>( )>(
			  "-9223372036854775808" );
		}

		constexpr auto get( wchar_t, std::integral_constant<size_t, 8> ) noexcept {
			return daw::basic_bounded_string<wchar_t, int_string_sizes::get<8>( )>(
			  L"-9223372036854775808" );
		}
	} // namespace min_strings

	namespace impl {
		template<typename>
		struct char_traits;

		template<>
		struct char_traits<char> {
			static constexpr char const decimal_point = '.';

			static constexpr daw::basic_string_view<char> nan( ) {
				return "nan";
			}

			static constexpr daw::basic_string_view<char> inf( ) {
				return "inf";
			}

			template<typename T>
			static constexpr char get_char_digit( T value ) noexcept {
				daw::exception::dbg_precondition_check(
				  0 <= value && value <= 9, "Only valid single digit numbers" );
				return '0' + static_cast<char>( value );
			}
		};

		template<>
		struct char_traits<wchar_t> {
			static constexpr wchar_t const decimal_point = L'.';

			static constexpr daw::basic_string_view<wchar_t> nan( ) {
				return L"nan";
			}

			static constexpr daw::basic_string_view<wchar_t> inf( ) {
				return L"inf";
			}

			template<typename T>
			static constexpr wchar_t get_char_digit( T value ) {
				return L'0' + static_cast<wchar_t>( value );
			}
		};
		template<
		  typename Result, typename Number,
		  std::enable_if_t<std::is_arithmetic_v<Number>, std::nullptr_t> = nullptr>
		constexpr uint16_t whole_log10( Number positive_value ) noexcept {
			Result result = 0;
			while( positive_value >= 10 ) {
				++result;
				positive_value /= static_cast<Number>( 10 );
			}
			return result;
		}
		// Integer numbers
		template<typename CharT, typename OutputIterator, typename Integer,
		         typename Traits = daw::impl::char_traits<CharT>,
		         std::enable_if_t<
		           daw::all_true_v<
		             not std::is_integral_v<daw::remove_cvref_t<Integer>>,
		             not std::is_same_v<bool, daw::remove_cvref_t<Integer>>,
		             not std::is_floating_point_v<daw::remove_cvref_t<Integer>>,
		             not daw::traits::is_character_v<Integer>>,
		           std::nullptr_t> = nullptr>
		constexpr OutputIterator to_os_string( OutputIterator it, Integer value,
		                                       daw::tag_t<int> ) {

			if( value < 0 ) {
				if( value == lowest_value<Integer> ) {
					constexpr auto m = min_strings::get(
					  CharT{ }, std::integral_constant<size_t, sizeof( Integer )>{ } );
					return daw::algorithm::copy( m.begin( ), m.end( ), it );
				}
				*it = static_cast<CharT>( '-' );
				++it;
				value = -value;
			}
			for( auto p10 = pow10<Integer>( whole_log10<uint16_t>( value ) );
			     p10 >= 1; p10 /= static_cast<Integer>( 10 ) ) {

				auto const tmp = value / p10;

				daw::exception::dbg_precondition_check(
				  tmp >= 0 && tmp < 10,
				  "There should only ever be a single digit positive number" );

				*it = Traits::get_char_digit( tmp );
				++it;

				value -= tmp * p10;
			}
			return it;
		}
	} // namespace impl
	template<typename Integer>
	inline constexpr bool can_to_os_string_int_v =
	  daw::all_true_v<std::is_integral_v<daw::remove_cvref_t<Integer>>,
	                  not std::is_same_v<bool, daw::remove_cvref_t<Integer>>,
	                  not std::is_floating_point_v<daw::remove_cvref_t<Integer>>,
	                  not daw::traits::is_character_v<Integer>>;

	template<
	  typename CharT, typename OutputIterator, typename Integer,
	  typename Traits = daw::impl::char_traits<CharT>,
	  std::enable_if_t<can_to_os_string_int_v<daw::remove_cvref_t<Integer>>,
	                   std::nullptr_t> = nullptr>
	constexpr OutputIterator to_os_string_int( OutputIterator it,
	                                           Integer value ) {
		return impl::to_os_string<CharT>( it, value, daw::tag<int> );
	}
} // namespace daw

// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <array>
#include <string>

#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	template<typename CharT, typename OutputCallback>
	class basic_output_stream {
		OutputCallback m_out;

	public:
		using reference = basic_output_stream &;

		constexpr explicit basic_output_stream( OutputCallback const &oi ) noexcept(
		  daw::is_nothrow_copy_constructible_v<OutputCallback> )
		  : m_out( oi ) {}

		constexpr reference operator( )( basic_string_view<CharT> str ) noexcept(
		  noexcept( std::declval<OutputCallback>( )( std::declval<CharT>( ) ) ) ) {

			m_out( str.data( ), str.size( ) );
			return *this;
		}

		constexpr reference operator( )( CharT const *str, size_t size ) noexcept(
		  noexcept( std::declval<OutputCallback>( )( std::declval<CharT>( ) ) ) ) {

			m_out( str, size );
			return *this;
		}

		constexpr reference operator( )( CharT c ) noexcept(
		  noexcept( std::declval<OutputCallback>( )( std::declval<CharT>( ) ) ) ) {

			m_out( c );
			return *this;
		}
	};

	namespace impl {
		template<typename CharT>
		constexpr size_t strlen( CharT const *ptr ) noexcept {
			size_t result = 0;
			while( ptr[result] != '\0' ) {
				++result;
			}
			return result;
		}
		 		
		namespace tostring_helpers {
			using std::to_string;

			template<typename CharT>
			constexpr basic_string_view<CharT>
			to_string( CharT const *str ) noexcept {
				return basic_string_view<CharT>( str, daw::impl::strlen( str ) );
			}

			template<typename CharT, size_t N>
			constexpr basic_string_view<CharT>
			to_string( CharT const ( &str )[N] ) noexcept {
				return basic_string_view<CharT>( str, N );
			}

			template<typename T>
			using has_tostring_detect =
			  decltype( to_string( std::declval<T const &>( ) ) );
		} // namespace tostring_helpers

		template<typename CharT, typename OutputCallback, typename T>
		using has_operator_lsh_lsh_detect = decltype( operator<<(
		  std::declval<daw::basic_output_stream<CharT, OutputCallback> &>( ),
		  std::declval<T const &>( ) ) );

		template<typename CharT, typename OutputCallback, typename T>
		constexpr bool has_operator_lsh_lsh_v =
		  daw::is_detected_v<has_operator_lsh_lsh_detect, CharT, OutputCallback, T>;

		template<typename T>
		constexpr bool has_tostring_v =
		  daw::is_detected_v<tostring_helpers::has_tostring_detect, T>;

		template<typename CharT>
		struct get_zero;

		template<>
		struct get_zero<char> {
			constexpr char operator( )( ) const noexcept {
				return '0';
			}
		};

		template<>
		struct get_zero<wchar_t> {
			constexpr char operator( )( ) const noexcept {
				return L'0';
			}
		};

		struct display_value {

			template<typename CharT, typename OutputCallback, typename T,
			         std::enable_if_t<(!daw::is_integral_v<std::decay_t<T>>),
			                          std::nullptr_t> = nullptr>
			static constexpr void
			display( basic_output_stream<CharT, OutputCallback> &os, T &&value ) {

				using impl::tostring_helpers::to_string;
				using std::to_string;
				auto str = to_string( std::forward<T>( value ) );
				os( str.data( ), str.size( ) );
			}

			template<typename T>
			static constexpr size_t num_digits( T value ) noexcept {
				static_assert( daw::is_integral_v<std::decay_t<T>>,
				               "value must be an integer type" );
				if( value == 0 ) {
					return 1;
				}
				size_t result = 0;
				while( value != 0 ) {
					value /= 10;
					++result;
				}
				return result;
			}

			template<typename CharT, typename OutputCallback, typename T,
			         std::enable_if_t<(daw::is_integral_v<std::decay_t<T>>),
			                          std::nullptr_t> = nullptr>
			static constexpr void
			display( basic_output_stream<CharT, OutputCallback> &os, T &&val ) {

				using int_t = std::decay_t<T>;
				std::array<char, num_digits( std::numeric_limits<int_t>::max( ) ) + 2>
				  buff = {0};
				int_t value = std::forward<T>( val );
				size_t pos_diff = 1;
				if( value < 0 ) {
					value *= -1;
					buff[0] = '-';
					pos_diff = 0;
				}

				auto const digit_count = num_digits( value );
				for( size_t n = 0; n < digit_count; ++n ) {
					auto const tmp = value / 10;
					auto const cur_digit = static_cast<CharT>( value - ( tmp * 10 ) );
					auto const cur_pos = ( digit_count - pos_diff ) - n;
					buff[cur_pos] = get_zero<CharT>{}( ) + cur_digit;
					value = tmp;
				}
				os( buff.data( ), digit_count + ( 1 - pos_diff ) );
			}

			template<typename CharT, typename OutputCallback>
			static constexpr void
			display( basic_output_stream<CharT, OutputCallback> &os, CharT const *str,
			         size_t N ) {

				os( str, N );
			}

			template<typename CharT, typename OutputCallback>
			static constexpr void
			display( basic_output_stream<CharT, OutputCallback> &os, CharT c ) {
				os( c );
			}
		};

		struct stdout_callable {
			inline void operator( )( char c ) const noexcept {
				putchar_unlocked( c );
			}

			inline void operator( )( char const *str, size_t count = 0 ) const
			  noexcept {
				if( count < 1 ) {
					count = daw::impl::strlen( str );
				}
				std::fwrite( static_cast<void const *>( str ), sizeof( char ), count,
				             stdout );
			}

			inline void operator( )( wchar_t c ) const noexcept {
				putwchar_unlocked( c );
			}

			inline void operator( )( wchar_t const *str, size_t count = 0 ) const
			  noexcept {
				if( count < 1 ) {
					count = daw::impl::strlen( str );
				}
				std::fwrite( static_cast<void const *>( str ), sizeof( wchar_t ), count,
				             stdout );
			}
		};
	} // namespace impl

	template<typename CharT, typename OutputCallback>
	constexpr auto make_output_stream( OutputCallback &&oi ) {
		return basic_output_stream<CharT, OutputCallback>(
		  std::forward<OutputCallback>( oi ) );
	}

	static auto con_out = make_output_stream<char>( impl::stdout_callable{} );
	static auto con_wout = make_output_stream<wchar_t>( impl::stdout_callable{} );
} // namespace daw

template<typename CharT, typename OutputCallback, typename T,
         std::enable_if_t<
           (daw::impl::has_tostring_v<T> &&
            !daw::impl::has_operator_lsh_lsh_v<CharT, OutputCallback, T>),
           std::nullptr_t> = nullptr>
constexpr daw::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::basic_output_stream<CharT, OutputCallback> &os,
            T const &value ) {

	daw::impl::display_value::display( os, value );
	return os;
}

template<typename CharT, typename OutputCallback, size_t N>
constexpr daw::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::basic_output_stream<CharT, OutputCallback> &os,
            CharT const ( &str )[N] ) {

	daw::impl::display_value::display( os, str, N - 1 );
	return os;
}

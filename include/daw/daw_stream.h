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

#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace io {
		namespace impl {
			template<typename String>
			using has_data_member_detect =
			  decltype( std::declval<String>( ).data( ) );

			template<typename String>
			using has_size_member_detect =
			  decltype( std::declval<String>( ).size( ) );

			template<typename String>
			constexpr bool is_string_like_v =
			  daw::is_detected_v<has_data_member_detect, String>
			    &&daw::is_detected_v<has_size_member_detect, String>;
		} // namespace impl

		template<typename CharT, typename OutputCallback>
		class basic_output_stream {
			OutputCallback m_out;

		public:
			using reference = basic_output_stream &;

			constexpr explicit basic_output_stream(
			  OutputCallback const
			    &oi ) noexcept( daw::is_nothrow_copy_constructible_v<OutputCallback> )
			  : m_out( oi ) {}

			constexpr explicit basic_output_stream( OutputCallback &&oi ) noexcept(
			  daw::is_nothrow_move_constructible_v<OutputCallback> )
			  : m_out( std::move( oi ) ) {}

			static constexpr bool is_nothrow_on_char_v =
			  noexcept( m_out( std::declval<CharT>( ) ) );
			static constexpr bool is_nothrow_on_sv_v =
			  noexcept( m_out( std::declval<daw::basic_string_view<CharT>>( ) ) );

			template<typename String,
			         std::enable_if_t<(impl::is_string_like_v<String> &&
			                           !daw::is_same_v<CharT, String>),
			                          std::nullptr_t> = nullptr>
			constexpr reference
			operator( )( String &&str ) noexcept( is_nothrow_on_sv_v ) {
				static_assert( daw::is_same_v<std::decay_t<CharT>,
				                              std::decay_t<decltype( *str.data( ) )>>,
				               "str must contain CharT data" );

				m_out( daw::basic_string_view<CharT>( str.data( ), str.size( ) ) );
				return *this;
			}

			constexpr reference
			operator( )( CharT c ) noexcept( is_nothrow_on_char_v ) {

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
					return basic_string_view<CharT>( str, daw::io::impl::strlen( str ) );
				}

				template<typename CharT, size_t N>
				constexpr basic_string_view<CharT>
				to_string( CharT const ( &str )[N] ) noexcept {
					return basic_string_view<CharT>( str, N );
				}

				template<typename T>
				using has_tostring_detect2 =
				  decltype( to_string( std::declval<T const &>( ) ) );

				template<typename T>
				constexpr bool has_tostring2_v =
				  daw::is_detected_v<tostring_helpers::has_tostring_detect2, T>;

				template<typename String,
				         std::enable_if_t<(!has_tostring2_v<std::decay_t<String>> &&
				                           is_string_like_v<std::decay_t<String>>),
				                          std::nullptr_t> = nullptr>
				constexpr auto to_string( String &&str ) noexcept {
					using CharT = std::decay_t<decltype( *str.data( ) )>;
					return basic_string_view<CharT>( str.data( ), str.size( ) );
				}

				template<typename T>
				using has_tostring_detect =
				  decltype( to_string( std::declval<T const &>( ) ) );

				template<typename T>
				constexpr bool has_tostring_v =
				  daw::is_detected_v<tostring_helpers::has_tostring_detect, T>;

			} // namespace tostring_helpers

			template<typename CharT, typename OutputCallback, typename T>
			using has_operator_lsh_lsh_detect = decltype( operator<<(
			  std::declval<daw::io::basic_output_stream<CharT, OutputCallback> &>( ),
			  std::declval<T const &>( ) ) );

			template<typename CharT, typename OutputCallback, typename T>
			constexpr bool has_operator_lsh_lsh_v =
			  daw::is_detected_v<has_operator_lsh_lsh_detect, CharT, OutputCallback,
			                     T>;

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

					using daw::io::impl::tostring_helpers::to_string;
					using std::to_string;
					os( to_string( std::forward<T>( value ) ) );
				}

			private:
				template<typename T>
				static constexpr auto num_digits( T value ) noexcept {
					using int_t = std::decay_t<T>;
					static_assert( daw::is_integral_v<int_t>,
					               "value must be an integer type" );
					if( value == 0 ) {
						return static_cast<int_t>( 1 );
					}
					int_t result = 0;
					while( value != 0 ) {
						value /= 10;
						++result;
					}
					return result;
				}

				template<typename Int>
				static constexpr Int pow10( Int n ) noexcept {
					Int result = 1;
					while( n > 1 ) {
						result *= 10;
						--n;
					}
					return result;
				}

			public:
				template<typename CharT, typename OutputCallback, typename T,
				         std::enable_if_t<(daw::is_integral_v<std::decay_t<T>>),
				                          std::nullptr_t> = nullptr>
				static constexpr void
				display( basic_output_stream<CharT, OutputCallback> &os, T &&val ) {

					using int_t = std::decay_t<T>;
					int_t value = std::forward<T>( val );
					size_t pos_diff = 1;
					if( value < 0 ) {
						os( '-' );
						value *= -1;
					}

					auto max10 = pow10<int_t>( num_digits<int_t>( value ) );
					while( max10 > 0 ) {
						auto const tmp = ( value / max10 ) * max10;
						CharT const cur_digit =
						  get_zero<CharT>{}( ) + static_cast<CharT>( tmp / max10 );
						os( cur_digit );
						value -= tmp;
						max10 /= 10;
					}
				}

				template<typename CharT, typename OutputCallback>
				static constexpr void
				display( basic_output_stream<CharT, OutputCallback> &os,
				         CharT const *str, size_t N ) {

					os( daw::basic_string_view<CharT>( str, N ) );
				}

				template<typename CharT, typename OutputCallback>
				static constexpr void
				display( basic_output_stream<CharT, OutputCallback> &os, CharT c ) {
					os( c );
				}
			};

			struct stdout_callable {
				inline void operator( )( char c ) const noexcept {
					putchar( c );
				}

				inline void operator( )( wchar_t c ) const noexcept {
					putwchar( c );
				}

				template<typename CharT>
				inline void operator( )( daw::basic_string_view<CharT> str ) const
				  noexcept {
					for( auto c : str ) {
						putchar( c );
					}
				}
			};
		} // namespace impl

		template<typename CharT, typename OutputCallback>
		constexpr auto make_output_stream( OutputCallback &&oi ) {
			return basic_output_stream<CharT, OutputCallback>(
			  std::forward<OutputCallback>( oi ) );
		}

	} // namespace io

	static auto con_out =
	  daw::io::make_output_stream<char>( daw::io::impl::stdout_callable{} );

	static auto con_wout =
	  daw::io::make_output_stream<wchar_t>( daw::io::impl::stdout_callable{} );
} // namespace daw

template<typename CharT, typename OutputCallback, size_t N>
constexpr daw::io::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::io::basic_output_stream<CharT, OutputCallback> &os,
            CharT const ( &str )[N] ) {

	os( daw::basic_string_view<CharT>( str, N - 1 ) );
	return os;
}

template<typename CharT, typename OutputCallback>
constexpr daw::io::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::io::basic_output_stream<CharT, OutputCallback> &os, bool b ) {

	if( b ) {
		operator<<( os, "true" );
	} else {
		operator<<( os, "false" );
	}
	return os;
}

template<typename CharT, typename OutputCallback, typename T,
         std::enable_if_t<
           (daw::io::impl::tostring_helpers::has_tostring_v<T> &&
            !daw::io::impl::has_operator_lsh_lsh_v<CharT, OutputCallback, T>),
           std::nullptr_t> = nullptr>
constexpr daw::io::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::io::basic_output_stream<CharT, OutputCallback> &os,
            T &&value ) {

	daw::io::impl::display_value::display( os, value );
	return os;
}


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

			for( auto c : str ) {
				m_out( c );
			}
			return *this;
		}

		constexpr reference operator( )( CharT const *str, size_t size ) noexcept(
		  noexcept( std::declval<OutputCallback>( )( std::declval<CharT>( ) ) ) ) {

			for( size_t n = 0; n < size; ++n ) {
				m_out( str[n] );
			}
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
		  std::declval<basic_output_stream<CharT, OutputCallback> &>( ),
		  std::declval<T const &>( ) ) );

		template<typename CharT, typename OutputCallback, typename T>
		constexpr bool has_operator_lsh_lsh_v =
		  daw::is_detected_v<has_operator_lsh_lsh_detect, CharT, OutputCallback, T>;

		template<typename T>
		constexpr bool has_tostring_v =
		  daw::is_detected_v<tostring_helpers::has_tostring_detect, T>;

		struct display_value {

			template<typename CharT, typename OutputCallback, typename T>
			constexpr static void
			display( basic_output_stream<CharT, OutputCallback> &os, T &&value ) {

				using impl::tostring_helpers::to_string;
				using std::to_string;
				auto str = to_string( std::forward<T>( value ) );
				os( str.data( ), str.size( ) );
			}

			template<typename CharT, typename OutputCallback>
			constexpr static void
			display( basic_output_stream<CharT, OutputCallback> &os,
			         basic_string_view<CharT> str ) {
				os( str );
			}

			template<typename CharT, typename OutputCallback>
			constexpr static void
			display( basic_output_stream<CharT, OutputCallback> &os, CharT c ) {
				os( c );
			}
		};

		template<typename CharT>
		struct stdout_callable {
			inline void operator( )( CharT c ) const noexcept {
				std::putchar( c );
			}
		};

		template<>
		struct stdout_callable<wchar_t> {
			inline void operator( )( wchar_t c ) const noexcept {
				std::putwchar( c );
			}
		};
	} // namespace impl

	template<typename CharT, typename OutputCallback>
	constexpr auto make_output_stream( OutputCallback &&oi ) {
		return basic_output_stream<CharT, OutputCallback>(
		  std::forward<OutputCallback>( oi ) );
	}

	static auto con_out =
	  make_output_stream<char>( impl::stdout_callable<char>{} );
	static auto con_wout =
	  make_output_stream<wchar_t>( impl::stdout_callable<wchar_t>{} );
} // namespace daw

template<typename CharT, typename OutputCallback, typename T,
         std::enable_if_t<
           (daw::impl::has_tostring_v<T> &&
            !daw::impl::has_operator_lsh_lsh_v<CharT, OutputCallback, T>),
           std::nullptr_t> = nullptr>
constexpr daw::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::basic_output_stream<CharT, OutputCallback> &os, T &&value ) {

	daw::impl::display_value::display( os, std::forward<T>( value ) );
	return os;
}

template<typename CharT, typename OutputCallback, size_t N>
constexpr daw::basic_output_stream<CharT, OutputCallback> &
operator<<( daw::basic_output_stream<CharT, OutputCallback> &os,
            CharT const ( &str )[N] ) {

	daw::impl::display_value::display( os,
	                                   daw::basic_string_view<CharT>( str, N ) );
	return os;
}

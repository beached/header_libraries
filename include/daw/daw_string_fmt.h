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

#include <cstddef>
#include <string>
#include <utility>

#include "daw_exception.h"
#include "daw_move.h"
#include "daw_parser_helper_sv.h"
#include "daw_string_view.h"
#include "daw_static_string.h"
#include "daw_traits.h"

namespace daw {
	namespace string_fmt {
		namespace v1 {
			struct invalid_string_fmt_index {};
			namespace sf_impl {
				template<size_t N>
				std::string to_string( char const ( &str )[N] ) {
					return std::string{str};
				}

				template<typename T>
				using has_to_string = decltype( std::declval<T>( ).to_string( ) );

				template<typename T>
				constexpr bool has_to_string_v = daw::is_detected_v<has_to_string, T>;

				template<typename T>
				using can_cast_to_string =
				  decltype( static_cast<std::string>( std::declval<T>( ) ) );

				template<typename T>
				constexpr bool can_cast_to_string_v =
				  daw::is_detected_v<can_cast_to_string, T>;

				template<typename T,
				         std::enable_if_t<
				           all_true_v<has_to_string_v<T>, !can_cast_to_string_v<T>>,
				           std::nullptr_t> = nullptr>
				inline std::string to_string( T const &value ) {
					return value.to_string( );
				}

				template<typename T, std::enable_if_t<can_cast_to_string_v<T>,
				                                      std::nullptr_t> = nullptr>
				inline std::string to_string( T const &value ) {
					return static_cast<std::string>( value );
				}

				template<uint8_t Cnt, uint8_t Sz, typename... Args,
				         std::enable_if_t<( Cnt >= Sz ), std::nullptr_t> = nullptr>
				inline std::string get_arg_impl( uint8_t const, Args &&... ) {
					daw::exception::daw_throw<daw::invalid_string_fmt_index>( );
				}

				template<uint8_t Cnt, uint8_t Sz, typename Arg, typename... Args,
				         std::enable_if_t<( Cnt < Sz ), std::nullptr_t> = nullptr>
				std::string get_arg_impl( uint8_t const idx, Arg &&arg,
				                          Args &&... args ) {
					if( Cnt == idx ) {
						using daw::sf_impl::to_string;
						using std::to_string;
						return to_string( std::forward<Arg>( arg ) );
					}
					return get_arg_impl<Cnt + 1, Sz>( idx,
					                                  std::forward<Args>( args )... );
				}

				template<typename... Args>
				std::string get_arg( uint8_t const idx, Args &&... args ) {
					return get_arg_impl<0, sizeof...( Args )>(
					  idx, std::forward<Args>( args )... );
				}
			} // namespace sf_impl

			class fmt_t {
				std::string m_format_str;

			public:
				template<typename String,
				         std::enable_if_t<daw::is_convertible_v<String, std::string>,
				                          std::nullptr_t> = nullptr>
				fmt_t( String &&format_str )
				  : m_format_str{std::forward<String>( format_str )} {}

				template<size_t N>
				fmt_t( char const ( &format_str )[N] )
				  : m_format_str{std::string{format_str}} {}

				template<typename... Args>
				std::string operator( )( Args &&... args ) const {
					std::string result{};
					result.reserve( m_format_str.size( ) +
					                ( sizeof...( args ) * 4 ) ); // WAG for size
					daw::string_view sv{m_format_str};
					result += sv.pop_front( "{" );
					while( !sv.empty( ) ) {
						auto const idx_str = sv.pop_front( "}" );
						auto const idx =
						  daw::parser::parse_unsigned_int<uint8_t>( idx_str );
						result += sf_impl::get_arg( idx, std::forward<Args>( args )... );
						result += sv.pop_front( "{" ).to_string( );
					}
					return result;
				}
			};

			template<typename... Args>
			std::string fmt( std::string format_str, Args &&... args ) {
				return fmt_t{daw::move( format_str )}( std::forward<Args>( args )... );
			}
		} // namespace v1
	}   // namespace string_fmt
	using string_fmt::v1::fmt_t;
	using string_fmt::v1::fmt;
	using string_fmt::v1::invalid_string_fmt_index;
} // namespace daw

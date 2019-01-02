// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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
#include <cstdint>
#include <type_traits>
#include <utility>

#include "cpp_17.h"

namespace daw {
	struct endian {
#ifdef _WIN32
		static constexpr int const little = 0;
		static constexpr int const big = 1;
		static constexpr int const native = little;
#else
		static constexpr int const little = __ORDER_LITTLE_ENDIAN__;
		static constexpr int const big = __ORDER_BIG_ENDIAN__;
		static constexpr int const native = __BYTE_ORDER__;
#endif
		static_assert( little != big, "Both little and big endian are set" );
	};

	namespace impl {
		template<typename T>
		constexpr decltype( auto )
		swap_bytes( T &&value,
		            std::integral_constant<size_t, sizeof( uint8_t )> ) noexcept {
			return std::forward<T>( value );
		}

		template<typename T>
		constexpr auto
		swap_bytes( T &&value,
		            std::integral_constant<size_t, sizeof( uint16_t )> ) noexcept {
			return static_cast<std::decay_t<T>>(
			  static_cast<uint16_t>(
			    ( static_cast<uint16_t>( value ) >> static_cast<uint16_t>( 8 ) ) &
			    static_cast<uint16_t>( 0x00FF ) ) |
			  static_cast<uint16_t>(
			    ( static_cast<uint16_t>( value ) << static_cast<uint16_t>( 8 ) ) &
			    static_cast<uint16_t>( 0xFF00 ) ) );
		}

		template<typename T>
		constexpr auto
		swap_bytes( T &&value,
		            std::integral_constant<size_t, sizeof( uint32_t )> ) noexcept {
			return static_cast<std::decay_t<T>>(
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) >> static_cast<uint32_t>( 24 ) ) &
			    static_cast<uint32_t>( 0x0000'00FF ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) >> static_cast<uint32_t>( 8 ) ) &
			    static_cast<uint32_t>( 0x0000'FF00 ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) << static_cast<uint32_t>( 8 ) ) &
			    static_cast<uint32_t>( 0x00FF'0000 ) ) |
			  static_cast<uint32_t>(
			    ( static_cast<uint32_t>( value ) << static_cast<uint32_t>( 24 ) ) &
			    static_cast<uint32_t>( 0xFF00'0000 ) ) );
		}

		template<typename T>
		constexpr auto
		swap_bytes( T &&value,
		            std::integral_constant<size_t, sizeof( uint64_t )> ) noexcept {
			return static_cast<std::decay_t<T>>(
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 56 ) ) &
			    static_cast<uint64_t>( 0x0000'0000'0000'00FF ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 40 ) ) &
			    static_cast<uint64_t>( 0x0000'0000'0000'FF00 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 24 ) ) &
			    static_cast<uint64_t>( 0x0000'0000'00FF'0000 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) >> static_cast<uint64_t>( 8 ) ) &
			    static_cast<uint64_t>( 0x0000'0000'FF00'0000 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 8U ) ) &
			    static_cast<uint64_t>( 0x0000'00FF'0000'0000 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 24 ) ) &
			    static_cast<uint64_t>( 0x0000'FF00'0000'0000 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 40 ) ) &
			    static_cast<uint64_t>( 0x00FF'0000'0000'0000 ) ) |
			  static_cast<uint64_t>(
			    ( static_cast<uint64_t>( value ) << static_cast<uint64_t>( 56 ) ) &
			    static_cast<uint64_t>( 0xFF00'0000'0000'0000 ) ) );
		}
	} // namespace impl

	template<typename T, std::enable_if_t<(endian::native == endian::little &&
	                                       daw::is_integral_v<std::decay_t<T>>),
	                                      std::nullptr_t> = nullptr>
	constexpr decltype( auto ) to_little_endian( T &&value ) noexcept {
		return std::forward<T>( value );
	}

	template<typename T, std::enable_if_t<(endian::native != endian::little &&
	                                       daw::is_integral_v<std::decay_t<T>>),
	                                      std::nullptr_t> = nullptr>
	constexpr auto to_little_endian( T &&value ) noexcept {
		return impl::swap_bytes( std::forward<T>( value ),
		                         std::integral_constant<size_t, sizeof( T )>{} );
	}

	template<typename T, std::enable_if_t<(endian::native == endian::big &&
	                                       daw::is_integral_v<std::decay_t<T>>),
	                                      std::nullptr_t> = nullptr>
	constexpr decltype( auto ) to_big_endian( T &&value ) noexcept {
		return std::forward<T>( value );
	}

	template<typename T, std::enable_if_t<(endian::native != endian::big &&
	                                       daw::is_integral_v<std::decay_t<T>>),
	                                      std::nullptr_t> = nullptr>
	constexpr auto to_big_endian( T &&value ) noexcept {
		return impl::swap_bytes( std::forward<T>( value ),
		                         std::integral_constant<size_t, sizeof( T )>{} );
	}

	template<typename T>
	constexpr decltype( auto ) host_to_network_endian( T &&value ) noexcept {
		return to_big_endian( std::forward<T>( value ) );
	}

	template<typename T>
	constexpr auto network_to_host_endian( T &&value ) noexcept
	  -> std::enable_if_t<( endian::native == endian::big ), decltype( value )> {

		return std::forward<T>( value );
	}

	template<typename T>
	constexpr auto network_to_host_endian( T &&value ) noexcept
	  -> std::enable_if_t<( endian::native != endian::big ), T> {

		return to_little_endian( std::forward<T>( value ) );
	}
} // namespace daw

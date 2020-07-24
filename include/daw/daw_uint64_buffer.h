// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#include <cstdint>

namespace daw {
	enum class UInt64Buffer : std::uint64_t {};

	constexpr UInt64Buffer &operator<<=( UInt64Buffer &b,
	                                     std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp <<= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator>>=( UInt64Buffer &b,
	                                     std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp >>= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer operator<<( UInt64Buffer b,
	                                   std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp <<= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer operator>>( UInt64Buffer b,
	                                   std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp >>= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer &operator|=( UInt64Buffer &b,
	                                    std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp |= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator&=( UInt64Buffer &b,
	                                    std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp &= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer &operator^=( UInt64Buffer &b,
	                                    std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp ^= shift;
		b = static_cast<UInt64Buffer>( tmp );
		return b;
	}

	constexpr UInt64Buffer operator|( UInt64Buffer b,
	                                  std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp |= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer operator&( UInt64Buffer b,
	                                  std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp &= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr UInt64Buffer operator^( UInt64Buffer b,
	                                  std::uint64_t shift ) noexcept {
		std::uint64_t tmp = static_cast<std::uint64_t>( b );
		tmp ^= shift;
		return static_cast<UInt64Buffer>( tmp );
	}

	constexpr bool operator==( UInt64Buffer lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) == rhs;
	}

	constexpr bool operator==( std::uint64_t lhs, UInt64Buffer rhs ) noexcept {
		return rhs == static_cast<std::uint64_t>( rhs );
	}

	constexpr bool operator!=( UInt64Buffer lhs, std::uint64_t rhs ) noexcept {
		return static_cast<std::uint64_t>( lhs ) != rhs;
	}

	constexpr bool operator!=( std::uint64_t lhs, UInt64Buffer rhs ) noexcept {
		return rhs != static_cast<std::uint64_t>( rhs );
	}

	constexpr UInt64Buffer to_uint64_buffer( std::uint64_t v ) noexcept {
		return static_cast<UInt64Buffer>( v );
	}

	constexpr UInt64Buffer to_uint64_buffer( char const *ptr ) noexcept {
		std::uint64_t result = 0;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[7] ) )
		          << 56U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[6] ) )
		          << 48U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[5] ) )
		          << 40U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[4] ) )
		          << 32U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[3] ) )
		          << 24U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[2] ) )
		          << 16U;
		result |= static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint64_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt64Buffer>( result );
	}
} // namespace daw

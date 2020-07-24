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
#include <type_traits>
#include <utility>

namespace daw {
	template<typename Unsigned>
	struct uint_buffer_impl {
		static_assert( std::is_unsigned_v<Unsigned>,
		               "Only unsigned types are supported" );
		enum class type : Unsigned {};
	};

	template<typename Unsigned>
	using UIntBuffer = typename uint_buffer_impl<Unsigned>::type;

	using UInt64Buffer = UIntBuffer<std::uint64_t>;
	using UInt32Buffer = UIntBuffer<std::uint32_t>;
	using UInt16Buffer = UIntBuffer<std::uint16_t>;

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> &
	operator<<=( UIntBuffer<Unsigned> &b, UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp <<= shift;
		b = static_cast<UIntBuffer<Unsigned>>( tmp );
		return b;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> &
	operator>>=( UIntBuffer<Unsigned> &b, UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp >>= shift;
		b = static_cast<UIntBuffer<Unsigned>>( tmp );
		return b;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> operator<<( UIntBuffer<Unsigned> b,
	                                           UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp <<= shift;
		return static_cast<UIntBuffer<Unsigned>>( tmp );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> operator>>( UIntBuffer<Unsigned> b,
	                                           UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp >>= shift;
		return static_cast<UIntBuffer<Unsigned>>( tmp );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> &operator|=( UIntBuffer<Unsigned> &b,
	                                            UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp |= shift;
		b = static_cast<UIntBuffer<Unsigned>>( tmp );
		return b;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> &operator&=( UIntBuffer<Unsigned> &b,
	                                            UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp &= shift;
		b = static_cast<UIntBuffer<Unsigned>>( tmp );
		return b;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> &operator^=( UIntBuffer<Unsigned> &b,
	                                            UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp ^= shift;
		b = static_cast<UIntBuffer<Unsigned>>( tmp );
		return b;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> operator|( UIntBuffer<Unsigned> b,
	                                          UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp |= shift;
		return static_cast<UIntBuffer<Unsigned>>( tmp );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> operator&( UIntBuffer<Unsigned> b,
	                                          UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp &= shift;
		return static_cast<UIntBuffer<Unsigned>>( tmp );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned> operator^( UIntBuffer<Unsigned> b,
	                                          UnsignedInteger shift ) noexcept {
		UnsignedInteger tmp = static_cast<UnsignedInteger>( b );
		tmp ^= shift;
		return static_cast<UIntBuffer<Unsigned>>( tmp );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr bool operator==( UIntBuffer<Unsigned> lhs,
	                           UnsignedInteger rhs ) noexcept {
		return static_cast<UnsignedInteger>( lhs ) == rhs;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr bool operator==( UnsignedInteger lhs,
	                           UIntBuffer<Unsigned> rhs ) noexcept {
		return rhs == static_cast<UnsignedInteger>( rhs );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr bool operator!=( UIntBuffer<Unsigned> lhs,
	                           UnsignedInteger rhs ) noexcept {
		return static_cast<UnsignedInteger>( lhs ) != rhs;
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr bool operator!=( UnsignedInteger lhs,
	                           UIntBuffer<Unsigned> rhs ) noexcept {
		return rhs != static_cast<UnsignedInteger>( rhs );
	}

	template<typename Unsigned, typename UnsignedInteger>
	constexpr UIntBuffer<Unsigned>
	to_uint64_buffer( UnsignedInteger v ) noexcept {
		return static_cast<UIntBuffer<Unsigned>>( v );
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

	constexpr UInt32Buffer to_uint32_buffer( char const *ptr ) noexcept {
		std::uint32_t result = 0;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[3] ) )
		          << 24U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[2] ) )
		          << 16U;
		result |= static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint32_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt32Buffer>( result );
	}

	constexpr UInt16Buffer to_uint16_buffer( char const *ptr ) noexcept {
		std::uint16_t result = 0;
		result |= static_cast<std::uint16_t>( static_cast<unsigned char>( ptr[1] ) )
		          << 8U;
		result |=
		  static_cast<std::uint16_t>( static_cast<unsigned char>( ptr[0] ) );
		return static_cast<UInt16Buffer>( result );
	}
} // namespace daw

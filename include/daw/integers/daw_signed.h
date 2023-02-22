// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_arith_traits.h"
#include "daw/daw_attributes.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_likely.h"
#include "impl/daw_signed_clanggcc.h"
#include "impl/daw_signed_error_handling.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <exception>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::integers {
	namespace int_impl {
		template<typename Lhs, typename Rhs>
		inline constexpr bool size_fits_v = sizeof( Lhs ) <= sizeof( Rhs );

		template<std::size_t Bits, bool Signed>
		struct IntTypes;

		template<>
		struct IntTypes<128, true> {
			using type = daw::int128_t;
		};

		template<>
		struct IntTypes<128, false> {
			using type = daw::uint128_t;
		};

		template<>
		struct IntTypes<64, true> {
			using type = std::int64_t;
		};

		template<>
		struct IntTypes<64, false> {
			using type = std::uint64_t;
		};
		template<>
		struct IntTypes<32, true> {
			using type = std::int32_t;
		};

		template<>
		struct IntTypes<32, false> {
			using type = std::uint32_t;
		};

		template<>
		struct IntTypes<16, true> {
			using type = std::int16_t;
		};

		template<>
		struct IntTypes<16, false> {
			using type = std::uint16_t;
		};

		template<>
		struct IntTypes<8, true> {
			using type = std::int8_t;
		};

		template<>
		struct IntTypes<8, false> {
			using type = std::uint8_t;
		};

		template<std::size_t Bits, typename Integer>
		inline constexpr bool convertible_signed_int =
		  std::is_integral_v<Integer> and std::is_signed_v<Integer> and
		  ( ( sizeof( Integer ) * CHAR_BIT ) <= Bits );

		inline constexpr struct debug_checked_add_t {
			explicit debug_checked_add_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_add( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
				return wrapped_add( lhs, rhs );
#else
				return lhs + rhs;
#endif
			}
		} debug_checked_add{ };

		inline constexpr struct debug_checked_sub_t {
			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_sub( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
				return wrapped_sub( lhs, rhs );
#else
				return lhs - rhs;
#endif
			}
		} debug_checked_sub{ };

		inline constexpr struct debug_checked_mul_t {
			explicit debug_checked_mul_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_mul( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
				return wrapped_mul( lhs, rhs );
#else
				return lhs * rhs;
#endif
			}
		} debug_checked_mul{ };

		inline constexpr struct debug_checked_div_t {
			explicit debug_checked_div_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_div( lhs, rhs );
#else
				return lhs / rhs;
#endif
			}
		} debug_checked_div{ };

		inline constexpr struct debug_checked_rem_t {
			explicit debug_checked_rem_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_rem( lhs, rhs );
#else
				return lhs % rhs;
#endif
			}
		} debug_checked_rem{ };

		inline constexpr struct debug_checked_shl_t {
			explicit debug_checked_shl_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_shl( lhs, rhs );
#else
				return lhs << rhs;
#endif
			}
		} debug_checked_shl{ };
		inline constexpr struct debug_checked_shr_t {
			explicit debug_checked_shr_t( ) = default;

			template<typename T,
			         std::enable_if_t<int_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_shr( lhs, rhs );
#else
				return lhs >> rhs;
#endif
			}
		} debug_checked_shr{ };
	} // namespace int_impl

	template<std::size_t Bits>
	struct signed_integer {
		using value_type = typename int_impl::IntTypes<Bits, true>::type;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		value_type m_value{ };

	public:
		explicit signed_integer( ) = default;

		template<typename Integer,
		         std::enable_if_t<int_impl::convertible_signed_int<Bits, Integer>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer( Integer v ) noexcept
		  : m_value( static_cast<value_type>( v ) ) {}

		template<std::size_t OtherBits,
		         std::enable_if_t<( OtherBits < Bits ), std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer(
		  signed_integer<OtherBits> other ) noexcept
		  : m_value( other.m_value ) {}

		template<typename Arithmetic,
		         std::enable_if_t<std::is_arithmetic_v<Arithmetic>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE explicit constexpr operator Arithmetic( ) const noexcept {
			return static_cast<Arithmetic>( m_value );
		}

		template<std::size_t NewBits>
		DAW_ATTRIB_INLINE explicit constexpr
		operator signed_integer<NewBits>( ) const noexcept {
			using result_t = signed_integer<NewBits>;
			using new_value_t = typename result_t::value_type;
			return result_t( static_cast<new_value_t>( m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr reference get( ) noexcept {
			return m_value;
		}

		DAW_ATTRIB_INLINE constexpr const_reference get( ) const noexcept {
			return m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator!( ) const {
			return signed_integer( static_cast<value_type>( !m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator-( ) const {
			return signed_integer( int_impl::debug_checked_mul( m_value, -1 ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator~( ) const {
			return signed_integer( static_cast<value_type>( ~m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator+=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_add( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_add( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_add( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_add( signed_integer const &rhs ) const {
			return signed_integer( int_impl::wrapped_add( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_add( signed_integer const &rhs ) const {
			return m_value + rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator-=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_sub( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_sub( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_sub( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_sub( signed_integer const &rhs ) const {
			return signed_integer( int_impl::wrapped_sub( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_sub( signed_integer const &rhs ) const {
			return m_value - rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator*=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_mul( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_mul( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_mul( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_mul( signed_integer const &rhs ) const {
			return signed_integer( int_impl::wrapped_mul( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		uncheck_mul( signed_integer const &rhs ) const {
			return m_value * rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator/=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_div( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_div( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_div( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_div( signed_integer const &rhs ) const {
			return m_value / rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator%=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_rem( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_rem( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_rem( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_rem( signed_integer const &rhs ) const {
			return m_value % rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator<<=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_shl( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_shl( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_shl( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_shl( signed_integer const &rhs ) const {
			return m_value << rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator>>=( signed_integer const &rhs ) {
			m_value = int_impl::debug_checked_shr( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		checked_shr( signed_integer const &rhs ) const {
			return signed_integer( int_impl::checked_shr( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_shr( signed_integer const &rhs ) const {
			return m_value >> rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator|=( signed_integer const &rhs ) const noexcept {
			m_value |= rhs.m_value;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator&=( signed_integer const &rhs ) const noexcept {
			m_value &= rhs.m_value;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator^=( signed_integer const &rhs ) const noexcept {
			m_value ^= rhs.m_value;
			return *this;
		}
	};

	namespace int_impl {
		template<typename Integer, typename = void>
		struct signed_from_integer;

		template<typename Integer>
		struct signed_from_integer<
		  Integer, std::enable_if_t<sizeof( Integer ) == sizeof( std::int64_t )>> {
			using type = signed_integer<64>;
		};

		template<typename Integer>
		struct signed_from_integer<
		  Integer, std::enable_if_t<sizeof( Integer ) == sizeof( std::int32_t )>> {
			using type = signed_integer<32>;
		};

		template<typename Integer>
		struct signed_from_integer<
		  Integer, std::enable_if_t<sizeof( Integer ) == sizeof( std::int16_t )>> {
			using type = signed_integer<16>;
		};

		template<typename Integer>
		struct signed_from_integer<
		  Integer, std::enable_if_t<sizeof( Integer ) == sizeof( std::int8_t )>> {
			using type = signed_integer<8>;
		};

		template<typename Integer>
		using signed_from_integer_t = typename signed_from_integer<Integer>::type;
	} // namespace int_impl

	template<std::size_t LhsBits, std::size_t RhsBits>
	using int_result_t = signed_integer<( std::max )( LhsBits, RhsBits )>;

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator+( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs += rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator-( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs -= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator*( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs *= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator/( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs /= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator%( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs %= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator<<( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs <<= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator>>( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs >>= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator|( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs |= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator&( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs &= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr int_result_t<LhsBits, RhsBits>
	operator^( signed_integer<LhsBits> lhs, signed_integer<RhsBits> rhs ) {
		return lhs ^= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator==( signed_integer<LhsBits> lhs,
	                                             signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) ==
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator==( signed_integer<Bits> lhs,
	                                             Integer rhs ) {
		return lhs == int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator==( Integer lhs,
	                                             signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) == rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator!=( signed_integer<LhsBits> lhs,
	                                             signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) !=
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator!=( signed_integer<Bits> lhs,
	                                             Integer rhs ) {
		return lhs != int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator!=( Integer lhs,
	                                             signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) != rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator<( signed_integer<LhsBits> lhs,
	                                            signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) <
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator<( signed_integer<Bits> lhs,
	                                            Integer rhs ) {
		return lhs < int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator<( Integer lhs,
	                                            signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) < rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator<=( signed_integer<LhsBits> lhs,
	                                             signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) <=
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator<=( signed_integer<Bits> lhs,
	                                             Integer rhs ) {
		return lhs <= int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator<=( Integer lhs,
	                                             signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) <= rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator>( signed_integer<LhsBits> lhs,
	                                            signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) >
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator>( signed_integer<Bits> lhs,
	                                            Integer rhs ) {
		return lhs > int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator>( Integer lhs,
	                                            signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) > rhs;
	}

	template<std::size_t LhsBits, std::size_t RhsBits>
	DAW_ATTRIB_INLINE constexpr bool operator>=( signed_integer<LhsBits> lhs,
	                                             signed_integer<RhsBits> rhs ) {
		return int_result_t<LhsBits, RhsBits>( lhs ).get( ) >=
		       int_result_t<LhsBits, RhsBits>( rhs ).get( );
	}

	template<std::size_t Bits, typename Integer>
	DAW_ATTRIB_INLINE constexpr bool operator>=( signed_integer<Bits> lhs,
	                                             Integer rhs ) {
		return lhs >= int_impl::signed_from_integer_t<Integer>( rhs );
	}

	template<typename Integer, std::size_t Bits>
	DAW_ATTRIB_INLINE constexpr bool operator>=( Integer lhs,
	                                             signed_integer<Bits> rhs ) {
		return int_impl::signed_from_integer_t<Integer>( lhs ) >= rhs;
	}

	namespace literals {
		DAW_CONSTEVAL signed_integer<8>
		operator"" _i8( unsigned long long v ) noexcept {
			using int_t = std::int8_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<8>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<16>
		operator"" _i16( unsigned long long v ) noexcept {
			using int_t = std::int16_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<16>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<32>
		operator"" _i32( unsigned long long v ) noexcept {
			using int_t = std::int32_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<32>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<64>
		operator"" _i64( unsigned long long v ) noexcept {
			using int_t = std::int64_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<64>( static_cast<int_t>( v ) );
		}
	} // namespace literals
	using i8 = signed_integer<8>;
	using i16 = signed_integer<16>;
	using i32 = signed_integer<32>;
	using i64 = signed_integer<64>;
} // namespace daw::integers

namespace daw {
	using daw::integers::i16;
	using daw::integers::i32;
	using daw::integers::i64;
	using daw::integers::i8;
} // namespace daw

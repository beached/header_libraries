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
#include "impl/daw_signed_msvc.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <exception>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::integers {
	namespace sint_impl {
		template<typename Lhs, typename Rhs>
		inline constexpr bool size_fits_v = sizeof( Lhs ) <= sizeof( Rhs );

		template<std::size_t Bits, bool Signed>
		struct IntTypes;

#if defined( DAW_HAS_INT128 )
		template<>
		struct IntTypes<128, true> {
			using type = daw::int128_t;
		};

		template<>
		struct IntTypes<128, false> {
			using type = daw::uint128_t;
		};
#endif

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

		template<typename SignedInteger, typename Integer>
		inline constexpr bool convertible_signed_int =
		  std::is_integral_v<Integer> and std::is_signed_v<Integer> and
		  ( sizeof( Integer ) <= sizeof( SignedInteger ) );

		inline constexpr struct debug_checked_add_t {
			explicit debug_checked_add_t( ) = default;

			template<typename T,
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
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
			         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
			                          std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				return checked_shr( lhs, rhs );
#else
				return lhs >> rhs;
#endif
			}
		} debug_checked_shr{ };

	} // namespace sint_impl

	template<typename SignedInteger>
	struct signed_integer;

	namespace sint_impl {
		template<typename, typename = void>
		inline constexpr bool is_signed_integral_v = false;

		template<typename T>
		inline constexpr bool is_signed_integral_v<
		  T, std::enable_if_t<std::is_integral_v<T> and std::is_signed_v<T>>> =
		  true;

		template<
		  typename Lhs, typename Rhs,
		  std::enable_if_t<is_signed_integral_v<Lhs> and is_signed_integral_v<Rhs>,
		                   std::nullptr_t> = nullptr>
		using int_result_t =
		  typename std::conditional<( sizeof( Lhs ) >= sizeof( Rhs ) ),
		                            signed_integer<Lhs>, signed_integer<Rhs>>::type;
	} // namespace sint_impl

	using i8 = signed_integer<std::int8_t>;
	using i16 = signed_integer<std::int16_t>;
	using i32 = signed_integer<std::int32_t>;
	using i64 = signed_integer<std::int64_t>;

#if defined( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#elif defined( _MSC_VER ) and not defined( __clang__ )
#pragma warning( push )
#pragma warning( disable : 5030 )
#endif
	template<typename SignedInteger>
	struct [[clang::preferred_name( i8 ), clang::preferred_name( i16 ),
	         clang::preferred_name( i32 ), clang::preferred_name( i64 ),
	         gnu::preferred_name( i8 ), gnu::preferred_name( i16 ),
	         gnu::preferred_name( i32 ),
	         gnu::preferred_name( i64 )]] signed_integer {
		static_assert( std::is_integral_v<SignedInteger> and
		                 std::is_signed_v<SignedInteger>,
		               "Only signed integer types are supported" );
		using value_type = SignedInteger;
		using reference = value_type &;
		using const_reference = value_type const &;

		static DAW_CONSTEVAL signed_integer max( ) noexcept {
			return signed_integer( std::numeric_limits<value_type>::max( ) );
		}

		static DAW_CONSTEVAL signed_integer min( ) noexcept {
			return signed_integer( std::numeric_limits<value_type>::min( ) );
		}

	private:
		value_type m_value{ };

	public:
		explicit signed_integer( ) = default;

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer( I v ) noexcept
		  : m_value( static_cast<value_type>( v ) ) {}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer(
		  signed_integer<I> other ) noexcept
		  : m_value( other.value( ) ) {}

		/// Construct from a literal
		template<
		  typename I,
		  std::enable_if_t<not sint_impl::convertible_signed_int<value_type, I> and
		                     sint_impl::is_signed_integral_v<I>,
		                   std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CONSTEVAL signed_integer( I v )
		  : m_value( static_cast<value_type>( v ) ) {
			// We know I can be larger than max( )
			if( v > static_cast<I>( std::numeric_limits<value_type>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
		}

		template<typename Arithmetic,
		         std::enable_if_t<std::is_arithmetic_v<Arithmetic>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE explicit constexpr operator Arithmetic( ) const noexcept {
			return static_cast<Arithmetic>( m_value );
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<I, value_type>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE explicit constexpr operator signed_integer<I>( )
		  const noexcept {
			return signed_integer<I>( m_value );
		}

		DAW_ATTRIB_INLINE constexpr reference value( ) noexcept {
			return m_value;
		}

		DAW_ATTRIB_INLINE constexpr const_reference value( ) const noexcept {
			return m_value;
		}

		DAW_ATTRIB_INLINE constexpr bool operator not( ) const {
			return not m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator-( ) const {
			return signed_integer( sint_impl::debug_checked_mul( m_value, -1 ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator~( ) const {
			return signed_integer( static_cast<value_type>( ~m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator+=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_add( m_value, rhs.m_value );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator++( ) {
			m_value = sint_impl::debug_checked_add( m_value, value_type{ 1 } );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_add(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_add( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer wrapped_add(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_add( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_add(
		  signed_integer const &rhs ) const {
			return m_value + rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator++(int) {
			auto result = *this;
			operator++();
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator+=( I rhs ) {
			return *this += signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator-=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_sub( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator-=( I rhs ) {
			return *this -= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_sub(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_sub( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer wrapped_sub(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_sub( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_sub(
		  signed_integer const &rhs ) const {
			return m_value - rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator--( ) {
			m_value = sint_impl::debug_checked_sub( m_value, value_type{ 1 } );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator--( int ) {
			auto result = *this;
			operator--( );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator*=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_mul( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator*=( I rhs ) {
			return *this *= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_mul(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_mul( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer wrapped_mul(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_mul( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer uncheck_mul(
		  signed_integer const &rhs ) const {
			return m_value * rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator/=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_div( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator/=( I rhs ) {
			return *this /= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_div(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_div( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_div(
		  signed_integer const &rhs ) const {
			return m_value / rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator%=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_rem( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator%=( I rhs ) {
			return *this %= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_rem(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_rem( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_rem(
		  signed_integer const &rhs ) const {
			return m_value % rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator<<=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_shl( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator<<=( I rhs ) {
			return *this <<= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_shl(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_shl( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_shl(
		  signed_integer const &rhs ) const {
			return m_value << rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator>>=(
		  signed_integer const &rhs ) {
			m_value = sint_impl::debug_checked_shr( m_value, rhs.m_value );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator>>=( I rhs ) {
			return *this >>= signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer checked_shr(
		  signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_shr( m_value, rhs.m_value ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer unchecked_shr(
		  signed_integer const &rhs ) const {
			return m_value >> rhs.m_value;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator|=(
		  signed_integer const &rhs ) const noexcept {
			m_value |= rhs.m_value;
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator|=( I rhs ) {
			m_value |= static_cast<value_type>( rhs );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator&=(
		  signed_integer const &rhs ) const noexcept {
			m_value &= rhs.m_value;
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator&=( I rhs ) {
			m_value &= static_cast<value_type>( rhs );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator^=(
		  signed_integer const &rhs ) const noexcept {
			m_value ^= rhs.m_value;
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator^=( I rhs ) {
			m_value ^= static_cast<value_type>( rhs );
			return *this;
		}
	};

#if defined( __GNUC__ )
#pragma GCC diagnostic pop
#elif defined( _MSC_VER ) and not defined( __clang__ )
#pragma warning( pop )
#endif

	template<typename I,
	         std::enable_if_t<std::is_signed_v<I> and std::is_integral_v<I>,
	                          std::nullptr_t> = nullptr>
	signed_integer( I ) -> signed_integer<I>;

	// Addition
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result += result_t( rhs.value( ) );
		return result;
	}
	// Subtraction
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result -= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result -= result_t( rhs.value( ) );
		return result;
	}

	// Multiplication
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result *= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result *= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result *= result_t( rhs.value( ) );
		return result;
	}

	// Division
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result /= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result /= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result /= result_t( rhs.value( ) );
		return result;
	}

	// Remainder
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result %= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result %= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result %= result_t( rhs.value( ) );
		return result;
	}
	// Shift Left
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) <<= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) <<= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) <<= result_t( rhs.value( ) );
	}

	// Shift Right
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) >>= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) >>= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) >>= result_t( rhs.value( ) );
	}

	// Bitwise Or
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) |= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) |= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) |= result_t( rhs.value( ) );
	}

	// Bitwise And
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) &= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) &= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) &= result_t( rhs.value( ) );
	}

	// Bitwise Xor
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) ^= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) ^= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) ^= result_t( rhs.value( ) );
	}

	// Equal To
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator==( signed_integer<Lhs> lhs,
	                                             signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) == result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator==( signed_integer<Lhs> lhs,
	                                             Rhs &&rhs )
	  -> decltype( lhs.value( ) == rhs ) {
		return lhs.value( ) == rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator==( Lhs &&lhs,
	                                             signed_integer<Rhs> rhs )
	  -> decltype( lhs == rhs.value( ) ) {
		return lhs == rhs.value( );
	}

	// Not Equal To
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator!=( signed_integer<Lhs> lhs,
	                                             signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) != result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator!=( signed_integer<Lhs> lhs,
	                                             Rhs &&rhs )
	  -> decltype( lhs.value( ) != rhs ) {
		return lhs.value( ) != rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator!=( Lhs &&lhs,
	                                             signed_integer<Rhs> rhs )
	  -> decltype( lhs != rhs.value( ) ) {
		return lhs != rhs.value( );
	}

	// Less Than
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator<( signed_integer<Lhs> lhs,
	                                            signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) < result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator<( signed_integer<Lhs> lhs,
	                                            Rhs &&rhs )
	  -> decltype( lhs.value( ) < rhs ) {
		return lhs.value( ) < rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator<( Lhs &&lhs,
	                                            signed_integer<Rhs> rhs )
	  -> decltype( lhs < rhs.value( ) ) {
		return lhs < rhs.value( );
	}

	// Less Than or Equal To
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator<=( signed_integer<Lhs> lhs,
	                                             signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) <= result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator<=( signed_integer<Lhs> lhs,
	                                             Rhs &&rhs )
	  -> decltype( lhs.value( ) <= rhs ) {
		return lhs.value( ) <= rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator<=( Lhs &&lhs,
	                                             signed_integer<Rhs> rhs )
	  -> decltype( lhs <= rhs.value( ) ) {
		return lhs <= rhs.value( );
	}

	// Greater Than
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator>( signed_integer<Lhs> lhs,
	                                            signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) > result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator>( signed_integer<Lhs> lhs,
	                                            Rhs &&rhs )
	  -> decltype( lhs.value( ) > rhs ) {
		return lhs.value( ) > rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator>( Lhs &&lhs,
	                                            signed_integer<Rhs> rhs )
	  -> decltype( lhs > rhs.value( ) ) {
		return lhs > rhs.value( );
	}

	// Greater Than or Equal To
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator>=( signed_integer<Lhs> lhs,
	                                             signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) >= result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator>=( signed_integer<Lhs> lhs,
	                                             Rhs &&rhs )
	  -> decltype( lhs.value( ) >= rhs ) {
		return lhs.value( ) >= rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator>=( Lhs &&lhs,
	                                             signed_integer<Rhs> rhs )
	  -> decltype( lhs >= rhs.value( ) ) {
		return lhs >= rhs.value( );
	}

	// Logical And
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator and( signed_integer<Lhs> lhs,
	                                               signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) and result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator and( signed_integer<Lhs> lhs,
	                                               Rhs &&rhs )
	  -> decltype( lhs.value( ) and rhs ) {
		return lhs.value( ) and rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator and( Lhs &&lhs,
	                                               signed_integer<Rhs> rhs )
	  -> decltype( lhs and rhs.value( ) ) {
		return lhs and rhs.value( );
	}

	// Logical Or
	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr bool operator or( signed_integer<Lhs> lhs,
	                                              signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) or result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator or( signed_integer<Lhs> lhs,
	                                              Rhs &&rhs )
	  -> decltype( lhs.value( ) or rhs ) {
		return lhs.value( ) or rhs;
	}

	template<typename Lhs, typename Rhs>
	DAW_ATTRIB_INLINE constexpr auto operator or( Lhs &&lhs,
	                                              signed_integer<Rhs> rhs )
	  -> decltype( lhs or rhs.value( ) ) {
		return lhs or rhs.value( );
	}

	namespace literals {
		DAW_CONSTEVAL signed_integer<std::int8_t>
		operator"" _i8( unsigned long long v ) noexcept {
			using int_t = std::int8_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int8_t>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<std::int16_t>
		operator"" _i16( unsigned long long v ) noexcept {
			using int_t = std::int16_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int16_t>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<std::int32_t>
		operator"" _i32( unsigned long long v ) noexcept {
			using int_t = std::int32_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int32_t>( static_cast<int_t>( v ) );
		}

		DAW_CONSTEVAL signed_integer<std::int64_t>
		operator"" _i64( unsigned long long v ) noexcept {
			using int_t = std::int64_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int64_t>( static_cast<int_t>( v ) );
		}
	} // namespace literals

} // namespace daw::integers

namespace daw {
	using daw::integers::i16;
	using daw::integers::i32;
	using daw::integers::i64;
	using daw::integers::i8;
} // namespace daw

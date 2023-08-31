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
#include "impl/daw_signed_error_handling.h"
#include "impl/daw_signed_impl.h"

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <exception>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::integers {
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

	template<typename SignedInteger>
	struct [[DAW_PREF_NAME( i8 ), DAW_PREF_NAME( i16 ), DAW_PREF_NAME( i32 ),
	         DAW_PREF_NAME( i64 )]] signed_integer {
		static_assert( std::is_integral_v<SignedInteger> and
		                 std::is_signed_v<SignedInteger>,
		               "Only signed integer types are supported" );
		using value_type = SignedInteger;
		using reference = value_type &;
		using const_reference = value_type const &;

		[[nodiscard]] static DAW_CONSTEVAL signed_integer max( ) noexcept {
			return signed_integer( std::numeric_limits<value_type>::max( ) );
		}

		[[nodiscard]] static DAW_CONSTEVAL signed_integer min( ) noexcept {
			return signed_integer( std::numeric_limits<value_type>::min( ) );
		}

		struct private_t {
			value_type value{ };
		} m_private{ };

	public:
		explicit signed_integer( ) = default;

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr explicit signed_integer( I v ) noexcept
		  : m_private{ static_cast<value_type>( v ) } {}

		[[nodiscard]] static constexpr signed_integer
		from_bytes_le( unsigned char const *ptr ) noexcept {
			return signed_integer(
			  daw::integers::sint_impl::from_bytes_le<value_type>(
			    ptr, std::make_index_sequence<sizeof( value_type )>{ } ) );
		}

		[[nodiscard]] static constexpr signed_integer
		from_bytes_be( unsigned char const *ptr ) noexcept {
			return signed_integer(
			  daw::integers::sint_impl::from_bytes_be<value_type>(
			    ptr, std::make_index_sequence<sizeof( value_type )>{ } ) );
		}

		template<typename I>
		[[nodiscard]] static constexpr signed_integer
		checked_conversion( signed_integer<I> other ) {
			if constexpr( sizeof( I ) <= sizeof( value_type ) ) {
				return signed_integer( static_cast<value_type>( other.value( ) ) );
			} else {
				auto val = static_cast<value_type>( other.value( ) );
				if( DAW_UNLIKELY( val != other.value( ) ) ) {
					on_signed_integer_overflow( );
				}
				return signed_integer( val );
			}
		}

		template<typename I,
		         std::enable_if_t<std::is_integral_v<I> and std::is_signed_v<I>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr signed_integer
		checked_conversion( I other ) {
			if constexpr( sizeof( I ) <= sizeof( value_type ) ) {
				return signed_integer( static_cast<value_type>( other ) );
			} else {
				auto val = static_cast<value_type>( other );
				if( DAW_UNLIKELY( val != other ) ) {
					on_signed_integer_overflow( );
				}
				return signed_integer( val );
			}
		}

		template<typename I>
		[[nodiscard]] static constexpr signed_integer
		unchecked_conversion( signed_integer<I> other ) {
			return signed_integer( static_cast<value_type>( other.value( ) ) );
		}

		template<typename I,
		         std::enable_if_t<std::is_integral_v<I> and std::is_signed_v<I>,
		                          std::nullptr_t> = nullptr>
		static constexpr signed_integer unchecked_conversion( I other ) {
			return signed_integer( static_cast<value_type>( other ) );
		}

		template<typename I,
		         std::enable_if_t<( sizeof( I ) > sizeof( value_type ) ),
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE explicit constexpr signed_integer(
		  signed_integer<I> other ) noexcept
		  : m_private{ static_cast<value_type>( other.value( ) ) } {
			if constexpr( sizeof( I ) > sizeof( value_type ) ) {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
				if( DAW_UNLIKELY( value( ) != other.value( ) ) ) {
					on_signed_integer_overflow( );
				}
#endif
			}
		}

		template<typename I,
		         std::enable_if_t<( sizeof( I ) < sizeof( value_type ) ),
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer(
		  signed_integer<I> other ) noexcept
		  : m_private{ static_cast<value_type>( other.value( ) ) } {}

		/// Construct from a literal
		template<
		  typename I,
		  std::enable_if_t<not sint_impl::convertible_signed_int<value_type, I> and
		                     sint_impl::is_signed_integral_v<I>,
		                   std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE DAW_CONSTEVAL explicit signed_integer( I v )
		  : m_private{ static_cast<value_type>( v ) } {
			// We know I can be larger than max( )
			if( v > static_cast<I>( std::numeric_limits<value_type>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
		}

		template<typename Arithmetic,
		         std::enable_if_t<std::is_arithmetic_v<Arithmetic>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE explicit constexpr
		operator Arithmetic( ) const noexcept {
			return static_cast<Arithmetic>( value( ) );
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<I, value_type>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE explicit constexpr
		operator signed_integer<I>( ) const noexcept {
			return signed_integer<I>( value( ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference value( ) noexcept {
			return m_private.value;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		value( ) const noexcept {
			return m_private.value;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool operator not( ) const {
			return not value( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		operator-( ) const {
			return signed_integer( sint_impl::debug_checked_neg( value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_neg( ) const {
			return signed_integer( sint_impl::checked_neg( value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_neg( ) const {
			return signed_integer( -value( ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		operator~( ) const {
			return signed_integer( static_cast<value_type>( ~value( ) ) );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator+=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_add( value( ), rhs.value( ) );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator++( ) {
			value( ) = sint_impl::debug_checked_add( value( ), value_type{ 1 } );
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_add( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_add( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_add( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_add( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_add( signed_integer const &rhs ) const {
			return value( ) + rhs.value( );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator+=( I rhs ) {
			return *this += signed_integer( rhs );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator-=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_sub( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator-=( I rhs ) {
			return *this -= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_sub( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_sub( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_sub( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_sub( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_sub( signed_integer const &rhs ) const {
			return value( ) - rhs.value( );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &operator--( ) {
			value( ) = sint_impl::debug_checked_sub( value( ), value_type{ 1 } );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer operator--( int ) {
			auto result = *this;
			operator--( );
			return result;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator*=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_mul( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator*=( I rhs ) {
			return *this *= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_mul( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_mul( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		wrapped_mul( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::wrapped_mul( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		uncheck_mul( signed_integer const &rhs ) const {
			return value( ) * rhs.value( );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator/=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_div( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator/=( I rhs ) {
			return *this /= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_div( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_div( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_div( signed_integer const &rhs ) const {
			return value( ) / rhs.value( );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator%=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_rem( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator%=( I rhs ) {
			return *this %= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_rem( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_rem( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_rem( signed_integer const &rhs ) const {
			return value( ) % rhs.value( );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator<<=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_shl( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator<<=( I rhs ) {
			return *this <<= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_shl( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_shl( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_shl( signed_integer const &rhs ) const {
			return value( ) << rhs.value( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		overflowing_shl( signed_integer n ) const {
			if( n < 0 ) {
				on_signed_integer_overflow( );
				return *this;
			} else if( n == 0 ) {
				return *this;
			}
			n &= sizeof( value_type ) * CHAR_BIT - 1;
			return signed_integer( value( ) << n.value( ) );
		}

		template<typename I,
		         std::enable_if_t<std::is_integral_v<I>, std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		overflowing_shl( I n ) const {
			if( n < 0 ) {
				on_signed_integer_overflow( );
				return *this;
			} else if( n == 0 ) {
				return *this;
			}
			n &= sizeof( value_type ) * CHAR_BIT - 1;
			return signed_integer( value( ) << n );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator>>=( signed_integer const &rhs ) {
			value( ) = sint_impl::debug_checked_shr( value( ), rhs.value( ) );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator>>=( I rhs ) {
			return *this >>= signed_integer( rhs );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		checked_shr( signed_integer const &rhs ) const {
			return signed_integer( sint_impl::checked_shr( value( ), rhs.value( ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		unchecked_shr( signed_integer const &rhs ) const {
			return value( ) >> rhs.value( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		overflowing_shr( signed_integer n ) const {
			if( n < 0 ) {
				on_signed_integer_overflow( );
				return *this;
			} else if( n == 0 ) {
				return *this;
			}
			n &= sizeof( value_type ) * CHAR_BIT - 1;
			return signed_integer( value( ) >> n.value( ) );
		}

		template<typename I,
		         std::enable_if_t<std::is_integral_v<I>, std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		overflowing_shr( I n ) const {
			if( n < 0 ) {
				on_signed_integer_overflow( );
				return *this;
			} else if( n == 0 ) {
				return *this;
			}
			n &= sizeof( value_type ) * CHAR_BIT - 1;
			return signed_integer( value( ) >> n );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		rotate_left( std::size_t n ) const {
			return overflowing_shl( n ) |
			       overflowing_shr( sizeof( value_type ) * CHAR_BIT - n );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr signed_integer
		rotate_right( std::size_t n ) const {
			return overflowing_shr( n ) |
			       overflowing_shl( sizeof( value_type ) * CHAR_BIT - n );
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator|=( signed_integer const &rhs ) noexcept {
			value( ) |= rhs.value( );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator|=( I rhs ) {
			value( ) |= static_cast<value_type>( rhs );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator&=( signed_integer const &rhs ) const noexcept {
			value( ) &= rhs.value( );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator&=( I rhs ) {
			value( ) &= static_cast<value_type>( rhs );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr signed_integer &
		operator^=( signed_integer const &rhs ) const noexcept {
			value( ) ^= rhs.value( );
			return *this;
		}

		template<typename I,
		         std::enable_if_t<sint_impl::convertible_signed_int<value_type, I>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr signed_integer &operator^=( I rhs ) {
			value( ) ^= static_cast<value_type>( rhs );
			return *this;
		}
	};

	template<typename I,
	         std::enable_if_t<std::is_signed_v<I> and std::is_integral_v<I>,
	                          std::nullptr_t> = nullptr>
	signed_integer( I ) -> signed_integer<I>;

	// Addition
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator+( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result += result_t( rhs.value( ) );
		return result;
	}

	// Subtraction
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result += result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result -= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator-( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result -= result_t( rhs.value( ) );
		return result;
	}

	// Multiplication
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result *= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result *= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator*( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result *= result_t( rhs.value( ) );
		return result;
	}

	// Division
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result /= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result /= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator/( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result /= result_t( rhs.value( ) );
		return result;
	}

	// Remainder
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result %= result_t( rhs.value( ) );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs.value( ) );
		result %= result_t( rhs );
		return result;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator%( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		auto result = result_t( lhs );
		result %= result_t( rhs.value( ) );
		return result;
	}
	// Shift Left
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) <<= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) <<= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator<<( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) <<= result_t( rhs.value( ) );
	}

	// Shift Right
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) >>= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) >>= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator>>( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) >>= result_t( rhs.value( ) );
	}

	// Bitwise Or
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) |= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) |= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator|( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) |= result_t( rhs.value( ) );
	}

	// Bitwise And
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) &= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) &= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator&( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) &= result_t( rhs.value( ) );
	}

	// Bitwise Xor
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) ^= result_t( rhs.value( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( signed_integer<Lhs> lhs, Rhs rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs.value( ) ) ^= result_t( rhs );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr sint_impl::int_result_t<Lhs, Rhs>
	operator^( Lhs lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ) ^= result_t( rhs.value( ) );
	}

	// Equal To
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator==( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) == result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator==( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) == rhs ) {
		return lhs.value( ) == rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator==( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs == rhs.value( ) ) {
		return lhs == rhs.value( );
	}

	// Not Equal To
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator!=( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) != result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator!=( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) != rhs ) {
		return lhs.value( ) != rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator!=( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs != rhs.value( ) ) {
		return lhs != rhs.value( );
	}

	// Less Than
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) < result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator<( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) < rhs ) {
		return lhs.value( ) < rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator<( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs < rhs.value( ) ) {
		return lhs < rhs.value( );
	}

	// Less Than or Equal To
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<=( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) <= result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator<=( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) <= rhs ) {
		return lhs.value( ) <= rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator<=( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs <= rhs.value( ) ) {
		return lhs <= rhs.value( );
	}

	// Greater Than
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) > result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator>( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) > rhs ) {
		return lhs.value( ) > rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator>( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs > rhs.value( ) ) {
		return lhs > rhs.value( );
	}

	// Greater Than or Equal To
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>=( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) >= result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator>=( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) >= rhs ) {
		return lhs.value( ) >= rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator>=( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs >= rhs.value( ) ) {
		return lhs >= rhs.value( );
	}

	// Logical And
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator and( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) and result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator and( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) and rhs ) {
		return lhs.value( ) and rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator and( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs and rhs.value( ) ) {
		return lhs and rhs.value( );
	}

	// Logical Or
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator or( signed_integer<Lhs> lhs, signed_integer<Rhs> rhs ) {
		using result_t = sint_impl::int_result_t<Lhs, Rhs>;
		return result_t( lhs ).value( ) or result_t( rhs ).value( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator or( signed_integer<Lhs> lhs, Rhs &&rhs )
	  -> decltype( lhs.value( ) or rhs ) {
		return lhs.value( ) or rhs;
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	operator or( Lhs &&lhs, signed_integer<Rhs> rhs )
	  -> decltype( lhs or rhs.value( ) ) {
		return lhs or rhs.value( );
	}

	namespace literals {
		[[nodiscard]] DAW_CONSTEVAL signed_integer<std::int8_t>
		operator"" _i8( unsigned long long v ) noexcept {
			using int_t = std::int8_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int8_t>( static_cast<int_t>( v ) );
		}

		[[nodiscard]] DAW_CONSTEVAL signed_integer<std::int16_t>
		operator"" _i16( unsigned long long v ) noexcept {
			using int_t = std::int16_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int16_t>( static_cast<int_t>( v ) );
		}

		[[nodiscard]] DAW_CONSTEVAL signed_integer<std::int32_t>
		operator"" _i32( unsigned long long v ) noexcept {
			using int_t = std::int32_t;
			if( v > static_cast<unsigned long long>(
			          std::numeric_limits<int_t>::max( ) ) ) {
				on_signed_integer_overflow( );
			}
			return signed_integer<std::int32_t>( static_cast<int_t>( v ) );
		}

		[[nodiscard]] DAW_CONSTEVAL signed_integer<std::int64_t>
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

namespace std {
	template<typename T>
	struct numeric_limits<daw::integers::signed_integer<T>> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr bool has_denorm = false;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style =
		  std::round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		// Cannot reasonably guess as it's imp defined for signed
		// static constexpr bool is_modulo = true;
		static constexpr int digits =
		  static_cast<int>( sizeof( T ) * CHAR_BIT - 1 );

		static constexpr int digits10 = digits * 3 / 10;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		// Cannot reasonably define
		// static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		min( ) noexcept {
			return daw::integers::signed_integer<T>::min( );
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		max( ) noexcept {
			return daw::integers::signed_integer<T>::max( );
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		lowest( ) noexcept {
			return daw::integers::signed_integer<T>::min( );
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		epsilon( ) noexcept {
			return 0;
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		round_error( ) noexcept {
			return 0;
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		infinity( ) noexcept {
			return 0;
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		quiet_NaN( ) noexcept {
			return 0;
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		signalling_NaN( ) noexcept {
			return 0;
		}

		[[nodiscard]] static constexpr daw::integers::signed_integer<T>
		denorm_min( ) noexcept {
			return 0;
		}
	};
} // namespace std

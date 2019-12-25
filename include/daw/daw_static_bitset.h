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

#include <array>
#include <cstdint>
#include <iosfwd>
#include <iterator>
#include <vector>

#include "daw_bit.h"
#include "daw_bounded_array.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace bitset_impl {
		template<typename T>
		using half_max_t = std::conditional_t<
		  sizeof( T ) == 8, uint32_t,
		  std::conditional_t<sizeof( T ) == 4, uint16_t,
		                     std::conditional_t<sizeof( T ) == 2, uint8_t, void>>>;

		constexpr auto low_part( uintmax_t value ) noexcept {
			using value_t = half_max_t<uintmax_t>;
			auto const msb_mask =
			  daw::mask_msb<uintmax_t>( bsizeof<uintmax_t> - bsizeof<value_t> );

			return static_cast<value_t>( value bitand msb_mask );
		}

		constexpr auto high_part( uintmax_t value ) noexcept {
			using value_t = half_max_t<uintmax_t>;
			return static_cast<value_t>( value >> bsizeof<value_t> );
		}

		constexpr auto overflow( uintmax_t &carry ) noexcept {
			using value_t = half_max_t<uintmax_t>;
			auto result = low_part( carry );
			carry >>= bsizeof<value_t>;
			return result;
		}

		constexpr char nibble_to_ascii( uint8_t value ) noexcept {
			if( value >= 10U ) {
				return 'A' + static_cast<char>( value - 10U );
			}
			return '0' + static_cast<char>( value );
		}

		constexpr char get_low_nibble( uint8_t value ) noexcept {
			value &= 0x0F;
			return nibble_to_ascii( value );
		}

		constexpr char get_high_nibble( uint8_t value ) noexcept {
			value = ( value & 0xF0 ) >> 4U;
			return nibble_to_ascii( value );
		}

		struct fmt_binary_t {};
		struct fmt_hex_t {};
		struct fmt_decimal_t {};

		template<size_t BitWidth, typename value_t>
		constexpr size_t get_elements_needed( ) noexcept {
			static_assert( BitWidth > 0 );
			if( BitWidth <= bsizeof<value_t> ) {
				return 1U;
			}
			size_t result = BitWidth / bsizeof<value_t>;
			if( BitWidth % bsizeof<value_t> != 0 ) {
				++result;
			}
			return result;
		}

		template<size_t BitWidth, typename value_t>
		constexpr value_t get_diff_mask( ) noexcept {
			size_t bw = BitWidth;
			if( bw > bsizeof<value_t> ) {
				bw %= bsizeof<value_t>;
			} else if( bw < bsizeof<value_t> ) {
				bw = bsizeof<value_t> - bw;
			}
			if( bw == 0 ) {
				return std::numeric_limits<value_t>::max( );
			}
			return daw::mask_msb<value_t>( bw );
		}
	} // namespace bitset_impl
	inline constexpr bitset_impl::fmt_binary_t const fmt_binary{};
	inline constexpr bitset_impl::fmt_hex_t const fmt_hex{};
	inline constexpr bitset_impl::fmt_decimal_t const fmt_decimal{};

	template<size_t BitWidth>
	class static_bitset {
		static_assert( BitWidth > 0 );
		using value_t = uintmax_t;

		static constexpr size_t const m_element_capacity =
		  bitset_impl::get_elements_needed<BitWidth, value_t>( );

		bounded_array_t<value_t, m_element_capacity> m_data{};

		struct bit_address_t {
			value_t index;
			value_t bit;
		};

		constexpr bit_address_t get_address( size_t index ) noexcept {
			bit_address_t result{};
			result.index = static_cast<value_t>( index / daw::bsizeof<value_t> );

			result.bit =
			  static_cast<value_t>( index - result.index * daw::bsizeof<value_t> );

			return result;
		}

		static constexpr void set_bit( value_t &value, value_t bit ) noexcept {
			value |= static_cast<value_t>( bit << 1U );
		}

		static constexpr void clear_bit( value_t &value, value_t bit ) noexcept {
			value &= static_cast<value_t>( ~static_cast<value_t>( bit << 1U ) );
		}

		static constexpr bool get_bit( value_t &value, value_t bit ) noexcept {
			return ( value & static_cast<value_t>( bit << 1U ) ) != 0U;
		}

		static constexpr size_t count_ones( value_t value ) noexcept {
			size_t count = 0;
			while( value ) {
				value &= value - 1U;
				++count;
			}
			return count;
		}

		static constexpr size_t count_zeros( value_t value ) noexcept {
			return bsizeof<value_t> - count_ones( value );
		}

		template<size_t N>
		static constexpr size_t
		count_ones( std::array<value_t, N> const &values ) noexcept {
			size_t count = 0;
			for( size_t n = 0; n < N; ++n ) {
				count += count_ones( values[n] );
			}
			return count;
		}

		template<size_t N>
		static constexpr size_t
		count_zeros( std::array<value_t, N> const &values ) noexcept {
			size_t count = 0;
			for( size_t n = 0; n < N; ++n ) {
				count += count_zeros( values[n] );
			}
			return count;
		}

	public:
		struct static_bitset_iterator;
		struct static_bitset_const_iterator;
		using value_type = bool;
		using iterator = static_bitset_iterator;
		using const_iterator = static_bitset_const_iterator;
		using size_type = size_t;
		using difference_type = intmax_t;
		using pointer = iterator;
		using const_pointer = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		/// Construct a bitset of all zeros
		///
		constexpr static_bitset( ) noexcept = default;

		/// Construct a static_bitset from a set of uintmax_t integers
		/// \tparam Unsigned unsigned integer types convertable to uintmax_t
		/// \param value lowest bytes in bitset
		/// \param values bytes in bitset from lowest(left) to highest(right)
		template<typename... Unsigned>
		explicit static_bitset( uintmax_t value, Unsigned... values ) noexcept
		  : m_data{value, values...} {

			// Ensure that we do not have any data on the high bits that should not
			// be there
			m_data.back( ) &= bitset_impl::get_diff_mask<BitWidth, value_t>( );
		}

		/// Construct a bitset from a string of zeros and ones
		///
		/// \param binary_sv A string like type convertable to string_view that
		/// consists of zeros and ones
		explicit constexpr static_bitset( daw::string_view binary_sv ) {
			daw::exception::precondition_check<std::overflow_error>(
			  binary_sv.size( ) <= BitWidth );

			size_t cur_idx = 0U;
			while( !binary_sv.empty( ) and cur_idx < m_element_capacity ) {
				value_t tmp = 0U;
				value_t cur_val = 1U;
				for( size_t n = 0U; n < daw::bsizeof<value_t> and !binary_sv.empty( );
				     ++n ) {
					switch( binary_sv.pop_back( ) ) {
					case '1':
						tmp |= cur_val;
						break;
					case '0':
						break;
					default:
						throw std::invalid_argument( "binary_sv" );
					}
					cur_val *= 2ULL;
				}
				m_data[cur_idx++] = tmp;
			}
		}

		template<
		  size_t BitWidthOther,
		  std::enable_if_t<( BitWidth > BitWidthOther ), std::nullptr_t> = nullptr>
		constexpr static_bitset(
		  static_bitset<BitWidthOther> const &other ) noexcept
		  : m_data{other.m_data} {}

		/// Enable a specific bit in the bitset
		/// \param index bit position in set
		constexpr void set_bit( size_t index ) noexcept {
			auto const loc = get_address( index );
			set_bit( m_data[loc.index], loc.bit );
		}

		/// Disable a specific bit in the bitset
		/// \param index bit position in set
		constexpr void clear_bit( size_t index ) noexcept {
			auto const loc = get_address( index );
			clear_bit( m_data[loc.index], loc.bit );
		}

		/// Get a specific bit from the bitset
		/// \param index bit position in set
		constexpr bool get_bit( size_t index ) noexcept {
			auto const loc = get_address( index );
			return get_bit( m_data[loc.index], loc.bit );
		}

		/// Set all bits to zero
		constexpr void clear( ) noexcept {
			for( auto &v : m_data ) {
				v = 0U;
			}
		}

		struct static_bitset_iterator {};
		struct static_bitset_const_iterator {};
		struct reference {};
		struct const_reference {};

		constexpr size_t one_count( ) const noexcept {
			return count_ones( m_data );
		}

		constexpr size_t zero_count( ) const noexcept {
			return count_zeros( m_data );
		}

		std::string to_string( bitset_impl::fmt_binary_t = fmt_binary ) const {
			std::string result( BitWidth, '0' );
			size_t pos = m_element_capacity - 1;
			for( size_t index = 0; index < m_data.size( ); ++index ) {
				auto value = m_data[index];
				if( !value ) {
					pos -= daw::bsizeof<value_t>;
					continue;
				}
				while( value ) {
					auto tmp = value & 1U;
					if( tmp == 1 ) {
						result[pos] = '1';
					}
					value >>= 1U;
					--pos;
				}
			}
			return result;
		}

		std::string to_string( bitset_impl::fmt_hex_t ) const {
			std::string result( m_data.size( ) * sizeof( value_t ) * 2U, '0' );
			size_t pos = m_data.size( ) * sizeof( value_t ) * 2U - 1U;
			for( size_t index = 0; index < m_data.size( ); ++index ) {
				auto value = m_data[index];
				if( !value ) {
					pos -= sizeof( value_t ) * 2U;
					continue;
				}
				for( size_t bpos = 0; bpos < sizeof( value_t ); ++bpos ) {
					uint8_t const tmp = static_cast<uint8_t>( value & 0xFF );
					value >>= 8U;
					result[pos--] = bitset_impl::get_low_nibble( tmp );
					result[pos--] = bitset_impl::get_high_nibble( tmp );
				}
			}
			return result;
		}

		constexpr iterator begin( ) noexcept;
		constexpr const_iterator begin( ) const noexcept;
		constexpr const_iterator cbegin( ) const noexcept;

		constexpr iterator end( ) noexcept;
		constexpr const_iterator end( ) const noexcept;
		constexpr const_iterator cend( ) const noexcept;

		constexpr reverse_iterator rbegin( ) noexcept;
		constexpr const_reverse_iterator rbegin( ) const noexcept;
		constexpr const_reverse_iterator crbegin( ) const noexcept;

		constexpr reverse_iterator rend( ) noexcept;
		constexpr const_reverse_iterator rend( ) const noexcept;
		constexpr const_reverse_iterator crend( ) const noexcept;

		constexpr reference operator[]( size_t index ) noexcept;
		constexpr const_reference operator[]( size_t index ) const noexcept;

		// Bitwise operations

		template<size_t BitWidthRhs>
		constexpr static_bitset
		operator|=( static_bitset<BitWidthRhs> const &rhs ) noexcept {
			size_t const SZ = std::min( m_data.size( ), rhs.m_data.size( ) );
			for( size_t n = 0; n < SZ; ++n ) {
				m_data[n] |= rhs.m_data[n];
			}
			return *this;
		}

		template<size_t BitWidthRhs>
		constexpr static_bitset<std::max( BitWidth, BitWidthRhs )>
		operator|( static_bitset<BitWidthRhs> const &rhs ) const noexcept {
			if constexpr( BitWidth >= BitWidthRhs ) {
				static_bitset result( *this );
				result |= rhs;
				return result;
			} else {
				static_bitset<BitWidthRhs> result( rhs );
				result |= *this;
				return result;
			}
		}

		template<size_t BitWidthRhs>
		constexpr static_bitset &
		operator&=( static_bitset<BitWidthRhs> const &rhs ) noexcept {
			size_t const SZ = std::min( m_data.size( ), rhs.m_data.size( ) );
			size_t n = 0;
			for( ; n < SZ; ++n ) {
				m_data[n] &= rhs.m_data[n];
			}
			for( ; n < m_data.size( ); ++n ) {
				m_data[n] = 0U;
			}
			return *this;
		}

		template<size_t BitWidthRhs>
		constexpr static_bitset
		operator&( static_bitset<BitWidthRhs> const &rhs ) const noexcept {
			static_bitset result( *this );
			result &= rhs;
			return result;
		}

		template<size_t BitWidthRhs>
		constexpr static_bitset<std::max( BitWidth, BitWidthRhs )>
		operator^( static_bitset<BitWidthRhs> const &rhs ) const noexcept {
			static_bitset<std::max( BitWidth, BitWidthRhs )> result{};
			size_t n = 0;
			for( ; n < m_data.size( ) and n < rhs.m_data.size( ); ++n ) {
				result.m_data[n] = m_data[n] ^ rhs.m_data[n];
			}
			// A xor 0
			for( ; n < m_data.size( ); ++n ) {
				result.m_data[n] = m_data[n] ^ 0U;
			}
			for( ; n < rhs.m_data.size( ); ++n ) {
				result.m_data[n] = 0U ^ rhs.m_data[n];
			}
			return result;
		}

		constexpr static_bitset operator~( ) const noexcept {
			static_bitset result{};
			for( size_t n = 0; n < m_data.size( ); ++n ) {
				result.m_data[n] = ~m_data[n];
			}
			return result;
		}

		template<size_t BitWidthRhs>
		constexpr bool operator==( static_bitset<BitWidthRhs> const &rhs ) const
		  noexcept {
			size_t const last = std::min( m_data.size( ), rhs.m_data.size( ) );
			size_t n = 0;
			for( ; n < last; ++n ) {
				if( m_data[n] != rhs.m_data[n] ) {
					return false;
				}
			}
			for( ; n < m_data.size( ); ++n ) {
				if( m_data[n] != 0U ) {
					return false;
				}
			}
			for( ; n < rhs.m_data.size( ); ++n ) {
				if( 0U != rhs.m_data[n] ) {
					return false;
				}
			}
			return true;
		}

		template<size_t BitWidthRhs>
		constexpr bool operator!=( static_bitset<BitWidthRhs> const &rhs ) const
		  noexcept {
			size_t const last = std::min( m_data.size( ), rhs.m_data.size( ) );
			size_t n = 0;
			for( ; n < last; ++n ) {
				if( m_data[n] != rhs.m_data[n] ) {
					return true;
				}
			}
			for( ; n < m_data.size( ); ++n ) {
				if( m_data[n] != 0U ) {
					return true;
				}
			}
			for( ; n < rhs.m_data.size( ); ++n ) {
				if( 0U != rhs.m_data[n] ) {
					return true;
				}
			}
			return false;
		}

		constexpr static_bitset &operator<<=( size_t bits ) noexcept {
			if( bits >= BitWidth ) {
				clear( );
				return *this;
			}
			if( bits < bsizeof<value_t> ) {
				value_t carry = 0U;
				for( size_t n = 0; n < m_data.size( ); ++n ) {
					value_t next_carry =
					  ( m_data[n] & daw::mask_lsb<value_t>( bits ) ) >>
					  ( bsizeof<value_t> - bits );
					m_data[n] <<= bits;
					m_data[n] |= carry;
					carry = next_carry;
				}
				return *this;
			}
			if( bits == bsizeof<value_t> ) {
				for( size_t n = m_data.size( ); n > 0; --n ) {
					auto pos = n - 1;
					m_data[pos] = m_data[pos - 1];
				}
				m_data[0] = 0U;
				return *this;
			}
			while( bits >= bsizeof<value_t> ) {
				operator<<=( bsizeof<value_t> );
				bits -= bsizeof<value_t>;
			}
			return operator<<=( bits );
		}

		constexpr static_bitset &operator>>=( size_t bits ) noexcept {
			if( bits >= BitWidth ) {
				clear( );
				return *this;
			}
			if( bits == 0U ) {
				return *this;
			}
			if( bits < bsizeof<value_t> ) {
				value_t carry = 0U;
				for( size_t n = m_data.size( ); n > 0; --n ) {
					auto const pos = n - 1;
					value_t next_carry = ( m_data[pos] & daw::mask_msb<value_t>( bits ) )
					                     << ( bsizeof<value_t> - bits );
					m_data[pos] >>= bits;
					m_data[pos] |= carry;
					carry = next_carry;
				}
				return *this;
			}
			if( bits == bsizeof<value_t> ) {
				for( size_t n = 0; n < m_data.size( ); ++n ) {
					m_data[n] = m_data[n + 1];
				}
				m_data.back( ) = 0U;
				return *this;
			}
			while( bits >= bsizeof<value_t> ) {
				operator>>=( bsizeof<value_t> );
				bits -= bsizeof<value_t>;
			}
			return operator>>=( bits );
		}
	};

	template<size_t BitWidth>
	inline std::string to_string( static_bitset<BitWidth> const &bs ) {
		return bs.to_string( );
	}

	template<size_t BitWidth>
	inline std::ostream &operator<<( std::ostream &os,
	                                 static_bitset<BitWidth> const &bs ) {

		os << bs.to_string( );
		return os;
	}
} // namespace daw

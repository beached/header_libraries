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

#include <array>
#include <cstdint>
#include <iterator>
#include <vector>

#include "daw_bit.h"
#include "daw_string_view.h"
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
			auto const lower_mask =
			  daw::get_left_mask<uintmax_t>( bsizeof<uintmax_t> - bsizeof<value_t> );

			return static_cast<value_t>( value bitand lower_mask );
		}

		constexpr auto high_part( uintmax_t value ) noexcept {
			using value_t = half_max_t<uintmax_t>;
			auto const high_mask =
			  daw::get_right_mask<uintmax_t>( bsizeof<uintmax_t> - bsizeof<value_t> );

			return static_cast<value_t>( value bitand high_mask );
		}

		constexpr auto overflow( uintmax_t &carry ) noexcept {
			using value_t = half_max_t<uintmax_t>;
			auto result = low_part( carry );
			carry >>= bsizeof<value_t>;
			return result;
		}
	} // namespace bitset_impl
	template<size_t MaxBitCapacity>
	class static_bitset {
		static_assert( MaxBitCapacity > 0 );
		using value_t = bitset_impl::half_max_t<uintmax_t>;

		static constexpr size_t const m_element_capacity =
		  (MaxBitCapacity / daw::bsizeof<value_t>)+(
		    MaxBitCapacity -
		    ((MaxBitCapacity / daw::bsizeof<value_t>)*daw::bsizeof<value_t>));

		std::array<value_t, m_element_capacity> m_data{};

		struct bit_address_t {
			size_t index;
			size_t bit;
		};

		constexpr bit_address_t get_address( size_t index ) noexcept {
			bit_address_t result{};
			result.index = index / daw::bsizeof<value_t>;
			result.bit = index - result.index * daw::bsizeof<value_t>;
			return result;
		}

		static constexpr void set_bit( value_t &value, size_t bit ) noexcept {}

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

		/// Construct a bitset from a unsigned integer
		///
		/// \param value An unsinged integer of the maximum size the system supports
		constexpr static_bitset( uintmax_t value ) noexcept
		  : m_data{bitset_impl::low_part( value ),
		           bitset_impl::high_part( value )} {}

		/// Construct a bitset from a string of zeros and ones
		///
		/// \param binary_sv A string like type convertable to string_view that
		/// consists of zeros and ones
		constexpr static_bitset( daw::string_view binary_sv ) {
			daw::exception::precondition_check<std::overflow_error>(
			  binary_sv.size( ) <= MaxBitCapacity );

			size_t cur_idx = 0;
			while( !binary_sv.empty( ) && cur_idx < m_element_capacity ) {
				value_t tmp = 0;
				value_t cur_val = 1U;
				for( size_t n = 0; n < daw::bsizeof<value_t> && !binary_sv.empty( );
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
					cur_val *= 2U;
				}
				m_data[cur_idx++] = tmp;
			}
		}

		constexpr void set_bit( size_t index ) noexcept {
			auto const loc = get_address( index );
		}
		struct static_bitset_iterator {};
		struct static_bitset_const_iterator {};
		struct reference {};
		struct const_reference {};
	};

} // namespace daw

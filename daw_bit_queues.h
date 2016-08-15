// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cassert>
#include <cstdint>

#include "daw_bit.h"

namespace daw {
	template<typename queue_type, typename value_type = uint8_t>
	class bit_queue_gen {
		queue_type m_queue;
		size_t m_size;

	public:
		bit_queue_gen( ): m_queue( 0 ), m_size( 0 ) { }

		size_t size( ) const {
			return m_size;
		}

		bool can_pop( size_t const bits ) const {
			return m_size >= bits;
		}

		bool empty( ) const {
			return 0 == m_size;
		}

		constexpr static size_t capacity( ) {
			return sizeof( m_queue ) * 8;
		}

		void push_back( value_type value, size_t const bits = sizeof( value_type ) * 8 ) {
			assert( (capacity( ) - m_size >= bits) && "Not enough bits to hold value pushed" );
			m_queue <<= bits;
			value &= get_mask<value_type>( bits );
			m_queue |= value;
			m_size += bits;
		}

		value_type pop_front( size_t const bits ) {
			queue_type const mask_pos = static_cast<queue_type>(m_size - (bits - 1));
			auto result = static_cast<value_type>(m_queue >> (mask_pos - 1)); // right shift so that all but the left most 6bits are gone
			queue_type const mask = static_cast<queue_type>(~(get_mask<queue_type>( bits - 1 ) << (m_size - bits)));
			m_queue &= mask;
			m_size -= bits;
			return result;
		}

		void clear( ) {
			m_queue = 0;
			m_size = 0;
		}

		value_type pop_all( ) {
			auto result = static_cast<value_type>(m_queue);
			clear( );
			return result;
		}
	};

	using bit_queue = bit_queue_gen<uint16_t>;

	template<typename queue_type, typename value_type = uint8_t>
	class nibble_queue_gen {
		bit_queue_gen<queue_type, value_type> m_queue;

	public:
		nibble_queue_gen( ): m_queue( ) { }

		constexpr static size_t capacity( ) {
			return sizeof( queue_type ) / 4;
		}

		size_t size( ) const {
			return m_queue.size( ) / 4;
		}

		bool empty( ) const {
			return 0 == size( );
		}

		void push_back( value_type const &value ) {
			m_queue.push_back( value, 4 );
		}

		void push_back( value_type const &value, size_t const & num_nibbles ) {
			m_queue.push_back( value, num_nibbles * 4 );
		}

		bool can_pop( size_t num_nibbles = sizeof( value_type ) * 2 ) const {
			return m_queue.can_pop( num_nibbles * 4 );
		}

		value_type pop_front( size_t num_nibbles = sizeof( value_type ) * 2 ) {
			return m_queue.pop_front( num_nibbles * 4 );
		}

		void clear( ) {
			m_queue.clear( );
		}

		value_type pop_all( ) {
			return m_queue.pop_all( );
		}
	};

	using nibble_queue = nibble_queue_gen<uint8_t>;
}

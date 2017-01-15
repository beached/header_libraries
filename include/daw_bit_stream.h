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

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>

#include "daw_bit_queues.h"

namespace daw {
	/*template<typename InputIteratorF, typename InputIteratorL> class bit_stream;

	template<typename BitStream>
	struct bit_stream_iterator: public std::input_iterator_tag {
		using value_type = typename std::iterator_traits<InputIteratorF>::value_type;
		using difference_type = typename std::iterator_traits<InputIteratorF>::difference_type;
		using reference = typename std::iterator_traits<InputIteratorF>::reference;
		using pointer = typename std::iterator_traits<InputIteratorF>::pointer;
	private:
		BitStream * m_bit_stream;
		bool m_is_end;

		bool at_end( ) const {
			return !m_bit_stream->valid( );
		}

		template<typename InputIteratorF, typename InputIteratorL> friend class bit_stream;

		bit_stream_iterator( BitStream * bit_stream, bool is_end ):
				m_bit_stream{ bit_stream },
				m_is_end{ is_end } { }
	public:
		bit_stream_iterator( BitStream & bit_stream ):
				bit_stream_iterator{ &bit_stream, !bit_stream } { }

		bool operator==( bit_stream_iterator const & rhs ) const {
			return (std::tie( m_bit_stream, m_bit_stream->m_first, m_bit_stream->m_left_overs ) == 
				std::tie( rhs.m_bit_stream, rhs.m_bit_stream->m_first, rhs.m_bit_stream->m_left_overs )) ||
				(is_end || !valid)
		}

		bool operator!=( bit_stream_iterator const & rhs ) const {
			return std::tie( m_bit_stream, m_bit_stream->m_first, m_bit_stream->m_left_overs ) !=
				std::tie( rhs.m_bit_stream, rhs.m_bit_stream->m_first, rhs.m_bit_stream->m_left_overs );
		}

		value_type operator*( ) {
			return m_bit
		}
	};*/

	template<typename InputIteratorF, typename InputIteratorL, typename BitQueueLSB = bit_queue_native_endian>
	class bit_stream {
		using value_type = std::decay_t<typename std::iterator_traits<InputIteratorF>::value_type>;
		static_assert(std::is_integral<value_type>::value, "value_type of iterator must be integral");

		template<typename BitStream> struct bit_stream_iterator;

		InputIteratorF m_first;
		InputIteratorL m_last;
		daw::bit_queue_gen<value_type, value_type, BitQueueLSB> m_left_overs;
	public:
		bit_stream( InputIteratorF first, InputIteratorL last ):
			m_first { first },
			m_last { last },
			m_left_overs { } { }

		bool valid( ) const {
			return m_first != m_last;
		}

		explicit operator bool( ) const noexcept {
			return valid( );
		}

		value_type pop_bits( size_t num_bits = sizeof( value_type ) ) {
			if( num_bits > sizeof( value_type )*8 ) {
				throw std::overflow_error( "Attempt to pop more bits than can fit into value" );
			}
			if( num_bits == 0 ) {
				throw std::runtime_error( "Attempt to pop 0 bits" );
			}
			value_type result = 0;

			if( num_bits > m_left_overs.size( ) ) {
				if( !m_left_overs.empty( ) ) {
					num_bits -= m_left_overs.size( );
					result = m_left_overs.pop_all( );
					result <<= num_bits;
				}
				if( m_first == m_last ) {
					throw std::out_of_range( "Attempt to iterate past last item" );
				}
				m_left_overs.push_back( *m_first++ );
			}
			result |= m_left_overs.pop_front( num_bits );
			return result;
		}

		void clear_left_overs( ) noexcept {
			m_left_overs.clear( );
		}
	};	// bit_stream

	template<typename BitQueueLSB = bit_queue_lsb_left, typename InputIteratorF, typename InputIteratorL>
	auto make_bit_stream( InputIteratorF first, InputIteratorL last ) {
		return bit_stream<InputIteratorF, InputIteratorL, BitQueueLSB>{ first, last };
	}
}


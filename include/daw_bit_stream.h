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
	template<typename ForwardIteratorF, typename ForwardIteratorL>
	class bit_stream {
		using value_type = std::decay_t<typename std::iterator_traits<IteratorF>::value_type>;
		static_assert(std::is_integral<value_type>::value, "value_type of iterator must be integral");

		ForwardIteratorF m_first;
		ForwardIteratorL m_last;
		daw::bit_queue_gen<value_type, value_type> m_left_overs;
	public:
		bit_stream( IteratorF first, IteratorL last ):
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
			if( num_bits > sizeof( value_type ) ) {
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
				m_left_overs.push_back( *m_first++ )
			}
			result |= m_left_overs.pop_front( num_bits );
			return result;
		}
	};	// bit_stream

	template<typename ForwardIteratorF, typename ForwardIteratorL>
	auto make_bit_stream( ForwardIteratorF first, ForwardIteratorL last ) {
		return bit_stream<ForwardIteratorF, ForwardIteratorL>{ first, last };
	}
}


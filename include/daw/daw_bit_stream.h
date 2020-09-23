// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_bit_queues.h"
#include "daw_exception.h"

#include <ciso646>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace daw {
	/*template<typename InputIteratorF, typename InputIteratorL> class bit_stream;

	template<typename BitStream>
	struct bit_stream_iterator: public std::input_iterator_tag {
	    using value_type = typename
	std::iterator_traits<InputIteratorF>::value_type; using difference_type =
	typename std::iterator_traits<InputIteratorF>::difference_type; using
	reference = typename std::iterator_traits<InputIteratorF>::reference; using
	pointer = typename std::iterator_traits<InputIteratorF>::pointer; private:
	    BitStream * m_bit_stream;
	    bool m_is_end;

	    bool at_end( ) const {
	        return !m_bit_stream->valid( );
	    }

	    template<typename InputIteratorF, typename InputIteratorL> friend class
	bit_stream;

	    bit_stream_iterator( BitStream * bit_stream, bool is_end ):
	            m_bit_stream{ bit_stream },
	            m_is_end{ is_end } { }
	public:
	    bit_stream_iterator( BitStream & bit_stream ):
	            bit_stream_iterator{ &bit_stream, !bit_stream } { }

	    bool operator==( bit_stream_iterator const & rhs ) const {
	        return (std::tie( m_bit_stream, m_bit_stream->m_first,
	m_bit_stream->m_left_overs ) == std::tie( rhs.m_bit_stream,
	rhs.m_bit_stream->m_first, rhs.m_bit_stream->m_left_overs )) or (is_end or
	!valid)
	    }

	    bool operator!=( bit_stream_iterator const & rhs ) const {
	        return std::tie( m_bit_stream, m_bit_stream->m_first,
	m_bit_stream->m_left_overs ) != std::tie( rhs.m_bit_stream,
	rhs.m_bit_stream->m_first, rhs.m_bit_stream->m_left_overs );
	    }

	    value_type operator*( ) {
	        return m_bit
	    }
	};*/

	template<typename InputIteratorF, typename InputIteratorL,
	         typename BitQueueLSB = bit_queue_source_native_endian>
	struct bit_stream {
		using value_type =
		  std::decay_t<typename std::iterator_traits<InputIteratorF>::value_type>;
		static_assert( std::is_integral<value_type>::value,
		               "value_type of iterator must be integral" );

	private:
		// template<typename BitStream> struct bit_stream_iterator;
		InputIteratorF m_first;
		InputIteratorL m_last;
		daw::basic_bit_queue<value_type, value_type, BitQueueLSB> m_left_overs;

	public:
		constexpr bit_stream( InputIteratorF first, InputIteratorL last ) noexcept
		  : m_first{ first }
		  , m_last{ last }
		  , m_left_overs{ } {}

		constexpr bool valid( ) const noexcept {
			return m_first != m_last;
		}

		explicit constexpr operator bool( ) const noexcept {
			return valid( );
		}

		value_type pop_bits( size_t num_bits = sizeof( value_type ) * 8 ) {
			daw::exception::dbg_throw_on_true<std::overflow_error>(
			  num_bits > sizeof( value_type ) * 8,
			  "Attempt to pop more bits than can fit into value" );
			daw::exception::dbg_throw_on_true<std::runtime_error>(
			  num_bits == 0, "Attempt to pop 0 bits" );

			value_type result = 0;

			if( num_bits > m_left_overs.size( ) ) {
				if( !m_left_overs.empty( ) ) {
					num_bits -= m_left_overs.size( );
					result = m_left_overs.pop_all( );
					result <<= num_bits;
				}
				daw::exception::dbg_throw_on_true<std::out_of_range>(
				  m_first == m_last, "Attempt to iterate past last item" );

				m_left_overs.push_back( *m_first++ );
			}
			result |= m_left_overs.pop_back( num_bits );
			return result;
		}

		constexpr void clear_left_overs( ) noexcept {
			m_left_overs.clear( );
		}
	}; // bit_stream

	template<typename T, typename BitStream>
	auto pop_value( BitStream &bs, size_t bits_needed ) {
		daw::exception::dbg_throw_on_false(
		  bits_needed <= sizeof( T ) * 8,
		  "Attempt to extra more bits than can fit" );

		using value_type = typename BitStream::value_type;
		T result = 0;
		while( bits_needed >= sizeof( value_type ) * 8 ) {
			result <<= sizeof( value_type ) * 8;
			result |= bs.pop_bits( );
			bits_needed -= sizeof( value_type ) * 8;
		}
		if( bits_needed > 0 ) {
			result <<= bits_needed;
			result |= bs.pop_bits( bits_needed );
		}
		return result;
	}

	template<typename T, typename BitStream>
	auto pop_value( BitStream &bs ) {
		return pop_value<T>( bs, sizeof( T ) * 8 );
	}

	template<typename BitStream>
	void skip_bits( BitStream &bs, size_t bits_needed ) {
		using value_type = typename BitStream::value_type;

		while( bits_needed >= sizeof( value_type ) * 8 ) {
			bs.pop_bits( );
			bits_needed -= sizeof( value_type ) * 8;
		}
	}

	template<typename BitStream>
	void skip_bytes( BitStream &bs, size_t bytes_needed ) {
		skip_bits( bs, bytes_needed * 8 );
	}

	template<typename BitStream, typename TestValue>
	void skip_until( BitStream &bs, TestValue const &v, size_t bit_count ) {
		daw::exception::dbg_throw_on_true(
		  bit_count > sizeof( TestValue ) * 8,
		  "Attempt to use more bits than can be put into TestValue" );
		daw::exception::dbg_throw_on_true( bit_count < 1,
		                                   "Attempt to pop less than 1 bits" );

		auto cur_val = pop_value<TestValue>( bs );
		while( bs and cur_val != v ) {
			cur_val = pop_value<TestValue>( bs );
		}
		daw::exception::dbg_throw_on_true( cur_val != v,
		                                   "Not enough data in stream" );
	}

	template<typename BitStream, typename TestValue>
	void skip_until( BitStream &bs, TestValue const &v ) {
		auto cur_val = bs.pop_bits( );
		while( bs and cur_val != v ) {
			cur_val = bs.pop_bits( );
		}
		daw::exception::dbg_throw_on_true( cur_val != v,
		                                   "Not enough data in stream" );
	}

	template<typename BitQueueLSB = bit_queue_source_native_endian,
	         typename InputIteratorF, typename InputIteratorL>
	constexpr auto make_bit_stream( InputIteratorF first,
	                                InputIteratorL last ) noexcept {
		return bit_stream<InputIteratorF, InputIteratorL, BitQueueLSB>{ first,
		                                                                last };
	}
} // namespace daw

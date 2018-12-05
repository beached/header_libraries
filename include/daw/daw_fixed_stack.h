// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_math.h"
#include "daw_static_array.h"

namespace daw {
	template<typename T, size_t N>
	struct fixed_stack_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = value_type *;
		using const_iterator = value_type const *;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = size_t;

	private:
		size_t m_index;
		size_t m_first;
		daw::static_array_t<T, N> m_stack;

	public:
		constexpr fixed_stack_t( ) noexcept
		  : m_index{0}
		  , m_first{0}
		  , m_stack{} {}

		constexpr fixed_stack_t( const_pointer ptr, size_type count ) noexcept
		  : m_index{daw::min( count, N )}
		  , m_first{0}
		  , m_stack{} {
			daw::algorithm::copy_n( ptr, m_stack.begin( ), daw::min( count, N ) );
		}

		constexpr bool empty( ) const noexcept {
			return m_index == m_first;
		}

		constexpr bool full( ) const noexcept {
			return m_index - m_first == N;
		}

		constexpr size_type size( ) const noexcept {
			return m_index - m_first;
		}

		constexpr size_type capacity( ) const noexcept {
			return N;
		}

		constexpr bool has_room( size_type count ) noexcept {
			return count + size( ) >= N;
		}

		constexpr size_type available( ) const noexcept {
			return N - size( );
		}

		constexpr void clear( ) noexcept {
			m_index = 0;
			m_first = 0;
		}

		constexpr reference front( ) noexcept {
			return m_stack[m_first];
		}

		constexpr const_reference front( ) const noexcept {
			return m_stack[m_first];
		}

		constexpr reference back( ) noexcept {
			return m_stack[m_index - 1];
		}

		constexpr const_reference back( ) const noexcept {
			return m_stack[m_index - 1];
		}

		constexpr reference operator[]( size_type pos ) noexcept {
			return m_stack[m_first + pos];
		}

		constexpr const_reference operator[]( size_type pos ) const noexcept {
			return m_stack[m_first + pos];
		}

		constexpr reference at( size_type pos ) {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		constexpr const_reference at( size_type pos ) const {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		constexpr pointer data( ) noexcept {
			return m_stack.data( ) + m_first;
		}

		constexpr const_pointer data( ) const noexcept {
			return m_stack.data( ) + m_first;
		}

		constexpr iterator begin( ) noexcept {
			return m_stack.begin( ) + m_first;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_stack.begin( ) + m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_stack.cbegin( ) + m_first;
		}

		constexpr iterator end( ) noexcept {
			return &m_stack[m_index];
		}

		constexpr const_iterator end( ) const noexcept {
			return &m_stack[m_index];
		}

		constexpr const_iterator cend( ) const noexcept {
			return &m_stack[m_index];
		}

	private:
		constexpr void do_move_to_front( ) noexcept {
			for( size_t n = m_first; n < m_index; ++n ) {
				m_stack[n - m_first] = m_stack[n];
			}
			m_index -= m_first;
			m_first = 0;
		}

		constexpr bool can_move_front( size_type how_many ) noexcept {
			return m_first > 0 and m_index >= N - how_many;
		}

	public:
		constexpr void push_back( const_reference value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] = value;
		}

		constexpr void push_back( value_type &&value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] = std::move( value );
		}

		constexpr void push_back( const_pointer ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = *ptr++;
			}
		}

		constexpr void assign( size_type count, const_reference value ) noexcept {
			clear( );
			for( size_t n = 0; n < count; ++n ) {
				push_back( value );
			}
		}

		template<typename Ptr>
		constexpr void push_back( Ptr const *ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = static_cast<value_type>( *ptr++ );
			}
		}

		template<typename... Args>
		constexpr void emplace_back( Args &&... args ) noexcept {
			if( can_move_front( sizeof...( Args ) ) ) {
				do_move_to_front( );
			}
			m_stack[m_index++] = value_type{std::forward<Args>( args )...};
		}

		constexpr value_type pop_back( ) noexcept {
			return m_stack[--m_index];
		}

		constexpr void pop_front( size_type const count ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_index <= m_index - count, "Attempt to pop_front past end of stack" );

			m_index += count;
		}

		///	take care calling as it is slow
		constexpr value_type pop_front( ) {
			auto result = front( );
			pop_front( 1 );
			return result;
		}

		constexpr void resize( size_type const count ) {

			daw::exception::precondition_check<std::out_of_range>(
			  count <= capacity( ), "Attempt to resize past capacity of fix_stack" );

			if( count > size( ) ) {
				if( can_move_front( count ) ) {
					do_move_to_front( );
				}
				for( size_type n = size( ); n < count; ++n ) {
					m_stack[n] = value_type{};
				}
			}
			m_index = count;
		}

		constexpr iterator erase( const_iterator pos ) {
				auto first = std::next( begin( ), std::distance( cbegin( ), pos ) );
				auto last = std::next( first, 1 );
				while( last != end( ) ) {
					daw::iter_swap( first, last );	
					std::advance( first, 1 );
					std::advance( last, 1 );
				}
				resize( size( ) - 1 );
				if( pos == cend( ) ) {
					return end( );
				}
				return std::next( begin( ), std::distance( cbegin( ), pos ) + 1 );
		}

		constexpr iterator erase( const_iterator first, const_iterator last ) {
				auto first1 = std::next( begin( ), std::distance( cbegin( ), first ) );
				auto first2 = std::next( begin( ), std::distance( cbegin( ), last) );
				while( first2 != end( ) ) {
					daw::iter_swap( first1, first2 );	
					std::advance( first1, 1 );
					std::advance( first2, 1 );
				}
				resize( size( ) - std::distance( first, last ) );
				if( last == cend( ) ) {
					return end( );
				}
				return std::next( begin( ), std::distance( cbegin( ), last ) + 1 );
		}

		constexpr void zero( ) noexcept {
			for( size_t n = 0; n < N; ++n ) {
				m_stack[n] = 0;
			}
		}
	};
} // namespace daw

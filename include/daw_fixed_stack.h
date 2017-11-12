// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include "daw_static_array.h"

namespace daw {
	template<typename T, size_t N>
	struct fixed_stack_t {
		using value_t = T;
		using reference = value_t &;
		using const_reference = value_t const &;
		using iterator = value_t *;
		using const_iterator = value_t const *;
		using pointer = value_t *;
		using const_pointer = value_t const *;
		using size_type = size_t;

	private:
		size_t m_index;
		daw::static_array_t<T, N> m_stack;

	public:
		constexpr fixed_stack_t( ) noexcept : m_index{0}, m_stack{} {}

		constexpr fixed_stack_t( const_pointer ptr, size_type count ) noexcept: m_index{ std::min( count, N ) }, m_stack{} {
			daw::algorithm::copy_n( ptr, std::min( count, N ), m_stack.begin( ) );
		}

		constexpr bool empty( ) const noexcept {
			return m_index == 0;
		}

		constexpr bool full( ) const noexcept {
			return m_index == N;
		}

		constexpr size_type size( ) const noexcept {
			return m_index;
		}

		constexpr size_type capacity( ) const noexcept {
			return N;
		}

		constexpr size_type available( ) const noexcept {
			return N - m_index;
		}

		constexpr void clear( ) noexcept {
			m_index = 0;
		}

		constexpr reference front( ) noexcept {
			return m_stack[0];
		}

		constexpr const_reference front( ) const noexcept {
			return m_stack[0];
		}

		constexpr reference back( ) noexcept {
			return m_stack[m_index - 1];
		}

		constexpr const_reference back( ) const noexcept {
			return m_stack[m_index - 1];
		}

		constexpr reference operator[]( size_type pos ) noexcept {
			return m_stack[pos];
		}

		constexpr const_reference operator[]( size_type pos ) const noexcept {
			return m_stack[pos];
		}
		constexpr pointer data( ) noexcept {
			return m_stack.data( );
		}

		constexpr const_pointer data( ) const noexcept {
			return m_stack.data( );
		}

		constexpr iterator begin( ) noexcept {
			return m_stack.begin( );
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_stack.begin( );
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_stack.cbegin( );
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

		constexpr void push_back( const_reference value ) noexcept {
			m_stack[m_index++] = value;
		}

		constexpr void push_back( const_pointer ptr, size_type sz ) noexcept {
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = *ptr++;
			}
		}

		constexpr void assign( size_type count, const_reference value ) noexcept {
			clear( );
			for( size_t n=0; n<count; ++n ) {
				push_back( value );
			}
		}

		template<typename Ptr>
		constexpr void push_back( Ptr const *ptr, size_type sz ) noexcept {
			auto const start = m_index;
			m_index += sz;
			for( size_t n = start; n < m_index; ++n ) {
				m_stack[n] = static_cast<value_t>( *ptr++ );
			}
		}

		template<typename... Args>
		constexpr void emplace_back( Args&&... args ) noexcept {
			m_stack[m_index++] = value_t{std::forward<Args>( args )...};
		}

		constexpr value_t pop_back( ) noexcept {
			return m_stack[--m_index];
		}

		constexpr void zero( ) noexcept {
			for( size_t n = 0; n < N; ++n ) {
				m_stack[n] = 0;
			}
		}
	};
} // namespace daw

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

#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace daw {
	template<typename T, size_t capacity>
	struct bounded_stack_t {
		static_assert( capacity >= 1, "There must be at least 1 element in stack capacity" );
		using value_type = std::decay_t<T>;
	private:
		using values_type = std::array<value_type, capacity>;
	public:
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;
	private:
		values_type m_values;
		iterator m_head;
		iterator m_last;

		const_iterator chead( ) const {
			return m_head;
		}
	public:
		constexpr bounded_stack_t( ) noexcept:
			m_values{ },
			m_head{ m_values.begin( ) } { }

		~bounded_stack_t( ) = default;
		bounded_stack_t( bounded_stack_t const & ) = default;
		bounded_stack_t( bounded_stack_t && ) = default;
		bounded_stack_t & operator=( bounded_stack_t const & ) = default;
		bounded_stack_t & operator=( bounded_stack_t && ) = default;

		constexpr void clear( ) const {
			m_head = m_values.begin( );
		}

		void clear( ) {
			m_head = m_values.begin( );
		}

		constexpr bool empty( ) const {
			return m_values.begin( ) == m_head;
		}

		constexpr size_t used( ) const {
			return static_cast<size_t>(std::distance( m_values.cbegin( ), chead( ) ));
		}
		
		constexpr size_t available( ) const {
			return static_cast<size_t>(std::distance( chead( ), m_values.cend( ) ));
		}

		constexpr bool full( ) const {
			return chead( ) == m_values.cend( );
		}
	
		void push_back( T value ) {
			if( full( ) ) {
				throw std::out_of_range( "Attempt to push_back on a full stack" );
			}
			*m_head++ = std::move( value );
		}	

		const_reference back( ) const {
			if( empty( ) ) {
				throw std::out_of_range( "Attempt to peek an empty stack" );
			}
			auto pos = m_head;
			return *--pos; 
		}

		reference front( ) {
			return m_values.front( );
		}

		const_reference front( ) const {
			return m_values.front( );
		}

		reference back( ) {
			if( empty( ) ) {
				throw std::out_of_range( "Attempt to peek an empty stack" );
			}
			auto pos = m_head;
			return *--pos; 
		}

		value_type pop_back( ) {
			if( empty( ) ) {
				throw std::out_of_range( "Attempt to pop an empty stack" );
			}
			return std::move( *--m_head ); 
		}
	};	// bounded_stack_t
}    // namespace daw


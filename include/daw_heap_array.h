// The MIT License (MIT)
//
// Copyright (c) 2013-2017 Darrell Wright
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

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace daw {
	template<typename T>
	struct heap_array {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;

	private:
		value_type *m_begin;
		value_type *m_end;
		size_t m_size;

	public:
		constexpr heap_array( ) noexcept : m_begin{nullptr}, m_end{nullptr}, m_size{0} {}

		heap_array( size_t Size ) : m_begin{new value_type[Size + 1]}, m_end{m_begin + Size}, m_size{Size} {}

		heap_array( size_t Size, value_type def_value )
		  : m_begin{new value_type[Size]}, m_end{m_begin + Size}, m_size{Size} {

			std::fill( m_begin, m_end, def_value );
		}

		constexpr heap_array( heap_array &&other ) noexcept = default;
		constexpr heap_array &operator=( heap_array && ) noexcept = default;

		heap_array( heap_array const &other )
		  : m_begin{other.m_size == 0 ? nullptr : new value_type[other.m_size]}
		  , m_end{other.m_size == 0 ? nullptr : m_begin + other.m_size}
		  , m_size{other.m_size} {

			std::copy_n( other.m_begin, m_size, m_begin );
		}

		heap_array &operator=( heap_array const &rhs ) {
			if( this != &rhs ) {
				clear( );
				if( rhs.m_begin == nullptr ) {
					return *this;
				}
				m_size = rhs.m_size;
				m_begin = new value_type[rhs.m_size];
				m_end = m_begin + m_size;
				std::copy_n( rhs.m_begin, m_size, m_begin );
			}
			return *this;
		}

		heap_array( std::initializer_list<value_type> values ) : heap_array( values.size( ) ) {

			std::copy_n( values.begin( ), values.size( ), m_begin );
		}

		heap_array &operator=( std::initializer_list<value_type> values ) {
			heap_array tmp{values.size( )};
			std::copy( values.begin( ), values.end( ), tmp.begin( ), tmp.end( ) );
			using std::swap;
			swap( *this, tmp );
			return *this;
		}

		heap_array( iterator arry, size_t Size ) : m_begin{new value_type[Size]}, m_end{m_begin + Size}, m_size{Size} {

			std::copy_n( arry, Size, m_begin );
		}

		void clear( ) noexcept {
			if( nullptr != m_begin ) {
				auto tmp = std::exchange( m_begin, nullptr );
				delete[] tmp;
				m_end = nullptr;
				m_size = 0;
			}
		}

		~heap_array( ) {
			clear( );
		}

		explicit operator bool( ) const noexcept {
			return nullptr == m_begin;
		}

		size_t size( ) const noexcept {
			return m_size;
		}

		bool empty( ) const noexcept {
			return m_begin == m_end;
		}

		iterator data( ) noexcept {
			return m_begin;
		}

		const_iterator data( ) const noexcept {
			return m_begin;
		}

		iterator begin( ) noexcept {
			return m_begin;
		}

		const_iterator begin( ) const noexcept {
			return m_begin;
		}

		const_iterator cbegin( ) const noexcept {
			return m_begin;
		}

		iterator end( ) noexcept {
			return m_end;
		}

		const_iterator end( ) const noexcept {
			return m_end;
		}

		const_iterator cend( ) const noexcept {
			return m_end;
		}

		reference operator[]( size_t pos ) {
			return *( m_begin + pos );
		}

		const_reference operator[]( size_t pos ) const {
			return *( m_begin + pos );
		}

		reference at( size_t pos ) {
			if( !( pos < m_size ) ) {
				throw std::out_of_range( "position is beyond end of heap_array" );
			}
			return operator[]( pos );
		}

		const_reference at( size_t pos ) const {
			if( !( pos < m_size ) ) {
				throw std::out_of_range( "position is beyond end of heap_array" );
			}
			return operator[]( pos );
		}

		reference front( ) {
			return *m_begin;
		}

		const_reference front( ) const {
			return *m_begin;
		}

		reference back( ) {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		const_reference back( ) const {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		iterator find_first_of( const_reference value, const_iterator start_at ) const {
			*m_end = value;
			while( *start_at != value ) {
				++start_at;
			}
			return const_cast<iterator>( start_at );
		}

		iterator find_first_of( const_reference value ) const {
			return find_first_of( value, cbegin( ) );
		}
	}; // struct heap_array

	template<typename T>
	void swap( daw::heap_array<T> &lhs, daw::heap_array<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}
} // namespace daw

// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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

#include <stdexcept>

namespace daw {
	template<typename T> struct array;
	
	template<typename T>
	void swap( array<T> & first, array<T> & second ) noexcept {
        using std::swap;
		swap( first.m_begin, second.m_begin );
		swap( first.m_end, second.m_end );
		swap( first.m_size, second.m_size );
    }

	template<typename T>
	struct array {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;
	private:
		value_type * m_begin;
		value_type * m_end;
		size_t m_size;

	public:
		friend void swap( array & first, array & second ) noexcept;

		array( ) noexcept: m_begin( nullptr ), m_end( nullptr ), m_size( 0 ) { }

		array( size_t count ): m_begin( nullptr ), m_end( nullptr ), m_size( count ) {
			m_begin = new value_type[m_size];
			m_end = m_begin + m_size;
		}

		array( size_t count, value_type def_value ): m_begin( nullptr ), m_end( nullptr ), m_size( count ) {
			m_begin = new value_type[m_size];
			m_end = m_begin + m_size;

			for( auto it = m_begin; it != m_end; ++it ) {
				*it = def_value;
			}
		}

		array( array && ) noexcept = default;
		array & operator=( array && ) noexcept = default;

		array( array const & other ):  m_begin( nullptr ), m_end( nullptr ), m_size( other.m_size ) {
			m_begin = new value_type[m_size];
			m_end = m_begin + m_size;
			std::copy( other.m_begin, other.m_end, m_begin );
		}

		array & operator=( array rhs ) noexcept {
			if( this != &rhs ) {
				swap( *this, rhs );	
			}
			return *this;
		}

		array( iterator arry, size_t count ): m_begin( nullptr ), m_end( nullptr ), m_size( count ) {
			m_begin = new value_type[m_size];
			m_end = m_begin + m_size;
			std::copy( arry, arry + count, m_begin );
		}

		~array( ) {
			if( m_begin ) {
				auto tmp = m_begin;
				m_begin = nullptr;
				m_end = nullptr;
				m_size = 0;
				delete[] tmp;
			}
		}

		explicit operator bool( ) const noexcept {
			return nullptr == m_begin;
		}

		size_t size( ) const noexcept {
			return m_size;
		}

		bool empty( ) const noexcept {
			return 0 == m_size;
		}
	
		iterator begin( ) noexcept {
			return m_begin;
		}

		const_iterator begin( ) const noexcept {
			return m_begin;
		}

		iterator end( ) noexcept {
			return m_end;
		}

		const_iterator end( ) const noexcept {
			return m_end;
		}

		reference operator[]( size_t pos ) {
			return *(m_begin + pos);
		}

		const_reference operator[]( size_t pos ) const {
			return *(m_begin + pos);
		}

		reference at( size_t pos ) {
			if( !(pos < m_size) ) {
				throw std::out_of_range( "position is beyond end of array" );
			}
			return *(m_begin + pos);
		}

		const_reference at( size_t pos ) const {
			if( !(pos < m_size) ) {
				throw std::out_of_range( "position is beyond end of array" );
			}
			return *(m_begin + pos);
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
	};	// struct array
}	// namespace daw


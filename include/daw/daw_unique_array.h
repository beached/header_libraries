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

#include <memory>
#include <stdexcept>

#include "daw_exception.h"

namespace daw {

	template<typename T>
	struct unique_array_t {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = size_t;
		using iterator = value_type *;
		using const_iterator = value_type const *;

	private:
		pointer m_data = nullptr;
		size_type m_size = 0;

	public:
		constexpr unique_array_t( ) noexcept = default;
		constexpr unique_array_t( unique_array_t && ) noexcept = default;
		constexpr unique_array_t &operator=( unique_array_t && ) noexcept = default;

		unique_array_t( size_type sz ) noexcept( noexcept( new value_type[sz]( ) ) )
		  : m_data{new value_type[sz]( )}
		  , m_size{sz} {}

		unique_array_t( size_type sz, const_reference default_value ) noexcept(
		  noexcept( new value_type[sz] ) )
		  : m_data{new value_type[sz]}
		  , m_size{sz} {

			for( size_type n = 0; n < m_size; ++n ) {
				m_data[n] = default_value;
			}
		}

		~unique_array_t( ) noexcept( noexcept( delete[] m_data ) ) {
			auto tmp = std::exchange( m_data, nullptr );
			m_size = 0;
			if( tmp ) {
				delete[] tmp;
			}
		}

		unique_array_t( unique_array_t const & ) = delete;
		unique_array_t &operator=( unique_array_t const & ) = delete;

		bool empty( ) const noexcept {
			return m_size == 0;
		}

		size_type size( ) const noexcept {
			return m_size;
		}

		size_type capacity( ) const noexcept {
			return m_size;
		}

		reference operator[]( size_type pos ) noexcept {
			return m_data[pos];
		}

		const_reference operator[]( size_type pos ) const noexcept {
			return m_data[pos];
		}

		reference at( size_type pos ) {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos >= m_size, "Attempt to access value past end of range" );
			return m_data[pos];
		}

		const_reference at( size_type pos ) const {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos >= m_size, "Attempt to access value past end of range" );
			return m_data[pos];
		}

		iterator begin( ) noexcept {
			return m_data;
		}

		const_iterator begin( ) const noexcept {
			return m_data;
		}

		const_iterator cbegin( ) const noexcept {
			return m_data;
		}

		iterator end( ) noexcept {
			return std::next( m_data, m_size );
		}

		const_iterator end( ) const noexcept {
			return std::next( m_data, m_size );
		}

		const_iterator cend( ) const noexcept {
			return std::next( m_data, m_size );
		}

		reference front( ) noexcept {
			return *m_data;
		}

		const_reference front( ) const noexcept {
			return *m_data;
		}

		reference back( ) noexcept {
			return *std::next( m_data, m_size - 1 );
		}

		const_reference back( ) const noexcept {
			return *std::next( m_data, m_size - 1 );
		}
	};
} // namespace daw

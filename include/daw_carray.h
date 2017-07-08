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

#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <stdexcept>

#include "daw_exception.h"

namespace daw {
	namespace details {
		template<typename T>
		constexpr void swap( T &lhs, T &rhs ) noexcept( noexcept( T( std::declval<T const &>( ) ) ) ) {
			T tmp = lhs;
			lhs = std::move( rhs );
			rhs = std::move( tmp );
			}
		} // namespace details

		template<typename T, size_t N>
		struct carray {
			using value_type = T;
			using reference = T &;
			using const_reference = T const &;
			using pointer = T *;
			using const_pointer = T const *;
			using size_type = std::size_t;

		  private:
			T m_data[N];

			template<typename Iterator>
			static constexpr std::reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
				return std::reverse_iterator<Iterator>( i );
			}
		  public:
			constexpr carray( ) noexcept = default; 

			template<typename... Args>
			constexpr carray( Args&&... args ): m_data{ std::forward<Args>(args)... } { }
			
			~carray( ) noexcept = default;

			constexpr carray( carray const &other ) noexcept( noexcept( T( std::declval<T const &>( ) ) ) ) {
				for( size_t n=0; n<N; ++n ) {
					m_data[n] = other.m_data[n];
				}
			}

			constexpr carray &operator=( carray const &other ) noexcept( noexcept( T( std::declval<T const &>( ) ) ) ) {
				for( size_t n=0; n<N; ++n ) {
					m_data[n] = other.m_data[n];
				}
			}

			constexpr carray( carray && ) noexcept( noexcept( T( std::declval<T &&>( ) ) ) ) = default;
			constexpr carray &operator=( carray && ) noexcept( noexcept( T( std::declval<T &&>( ) ) ) ) = default;

			constexpr reference operator[]( size_type pos ) noexcept {
				return m_data[pos];
			}

			constexpr const_reference operator[]( size_type pos ) const noexcept {
				return m_data[pos];
			}

			constexpr reference at( size_type pos ) {
				daw::exception::daw_throw_on_false<std::out_of_range>( pos < N, "Attemp to access past end of array" );
				return m_data[pos];
			}

			constexpr const_reference at( size_type pos ) const noexcept {
				daw::exception::daw_throw_on_false<std::out_of_range>( pos < N, "Attemp to access past end of array" );
				return m_data[pos];
			}

			constexpr pointer begin( ) noexcept {
				return m_data;
			}

			constexpr const_pointer begin( ) const noexcept {
				return m_data;
			}

			constexpr const_pointer cbegin( ) const noexcept {
				return m_data;
			}

			constexpr pointer end( ) noexcept {
				return &m_data[N];
			}

			constexpr const_pointer end( ) const noexcept {
				return &m_data[N];
			}

			constexpr const_pointer cend( ) const noexcept {
				return &m_data[N];
			}

			constexpr auto rbegin( ) noexcept {
				static_assert( N > 0, "Cannot call rbegin on empty array" );
				return make_reverse_iterator( &m_data[N-1] );
			}

			constexpr auto rbegin( ) const noexcept {
				static_assert( N > 0, "Cannot call rbegin on empty array" );
				return make_reverse_iterator( &m_data[N-1] );
			}

			constexpr auto crbegin( ) const noexcept {
				static_assert( N > 0, "Cannot call crbegin on empty array" );
				return make_reverse_iterator( &m_data[N-1] );
			}

			constexpr auto rend( ) noexcept {
				static_assert( N > 0, "Cannot call rend on empty array" );
				return make_reverse_iterator( m_data - 1 );
			}

			constexpr auto rend( ) const noexcept {
				static_assert( N > 0, "Cannot call rend on empty array" );
			return make_reverse_iterator( m_data - 1 );
		}

		constexpr auto crend( ) const noexcept {
			static_assert( N > 0, "Cannot call crend on empty array" );
			return make_reverse_iterator( m_data - 1 );
		}

		constexpr reference front( ) noexcept {
			static_assert( N > 0, "Cannot call front on empty array" );
			return m_data[0];
		}

		constexpr const_reference front( ) const noexcept {
			static_assert( N > 0, "Cannot call front on empty array" );
			return m_data[0];
		}

		constexpr reference back( ) noexcept {
			static_assert( N > 0, "Cannot call back on empty array" );
			return m_data[N-1];
		}
		constexpr const_reference back( ) const noexcept {
			static_assert( N > 0, "Cannot call back on empty array" );
			return m_data[N-1];
		}

		constexpr const_pointer data( ) const noexcept {
			return m_data;
		}

		constexpr size_type size( ) const noexcept {
			return N;
		}

		constexpr size_type max_size( ) const noexcept {
			return N;
		}

		constexpr bool empty( ) const noexcept {
			return 0 == N;
		}

		constexpr void fill( const_reference value ) noexcept( noexcept( T( std::declval<T const &>( ) ) ) ) {
			std::fill( begin( ), end( ), value );
		}

		constexpr void
		swap( carray &rhs ) noexcept( noexcept( details::swap( std::declval<T &>( ), std::declval<T &>( ) ) ) ) {
			using details::swap;
			for( size_t n=0; n<N; ++n ) {
				swap( m_data[n], rhs.m_data[n] );
			}
		}
	}; // carray

	template<typename T, size_t N>
	constexpr void
	swap( carray<T, N> &lhs,
	      carray<T, N> &rhs ) noexcept( noexcept( details::swap( std::declval<T &>( ), std::declval<T &>( ) ) ) ) {
		lhs.swap( rhs );
	}
}    // namespace daw



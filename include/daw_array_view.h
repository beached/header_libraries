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

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "daw_fnv1a_hash.h"
#include "daw_iterator.h"
#include "daw_reverse_iterator.h"
#include "daw_utility.h"

namespace daw {
	template<typename T>
	struct array_view {
		using value_type = std::remove_reference_t<T>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = daw::reverse_iterator<iterator>;
		using const_reverse_iterator = daw::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

	private:
		const_pointer m_first;
		size_type m_size;

	public:
		constexpr array_view( ) noexcept : m_first{nullptr}, m_size{0} {}

		explicit constexpr array_view( std::nullptr_t ) noexcept : m_first{nullptr}, m_size{0} {}
		constexpr array_view( std::nullptr_t, size_type ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr array_view( const_pointer s, size_type count ) noexcept : m_first{s}, m_size{count} {}

		template<size_t N>
		explicit constexpr array_view( value_type const ( &s )[N] ) noexcept : m_first{s}, m_size{N} {}

		constexpr array_view( array_view const &other ) noexcept = default;
		constexpr array_view &operator=( array_view const & ) noexcept = default;
		constexpr array_view &operator=( array_view && ) noexcept = default;

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return m_first[pos];
		}

		constexpr const_reference at( size_type const pos ) const {
			if( pos >= m_size ) {
				throw std::out_of_range{"Attempt to access array_view past end"};
			}
			return m_first[pos];
		}

		constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		constexpr const_reference back( ) const noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr bool empty( ) const noexcept {
			return nullptr == m_first || 0 == m_size;
		}

		constexpr void remove_prefix( size_type const n ) noexcept {
			m_first += n;
			m_size -= n;
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( size_type const n ) noexcept {
			m_size -= n;
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		constexpr T pop_front( ) noexcept {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		constexpr T pop_back( ) noexcept {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		constexpr bool pop( T &value ) noexcept {
			value = back( );
			remove_suffix( );
			return true;
		}

		constexpr void resize( size_type const n ) noexcept {
			m_size = n;
		}

		constexpr void swap( array_view &v ) noexcept {
			using std::swap;
			swap( m_first, v.m_first );
			swap( m_size, v.m_size );
		}

		constexpr size_type copy( T *dest, size_type const count, size_type const pos = 0 ) const {
			if( pos >= m_size ) {
				throw std::out_of_range{"Attempt to access array_view past end"};
			}
			size_type const rlen = ( std::min )( count, m_size - pos );
			auto src = m_first + pos;
			for( size_t n = 0; n < rlen; ++n ) {
				dest[n] = src[n];
			}
			return rlen;
		}

		constexpr array_view subset( size_type const pos = 0,
		                             size_type const count = std::numeric_limits<size_type>::max( ) ) const {
			if( pos >= size( ) ) {
				throw std::out_of_range{"Attempt to access array_view past end"};
			}
			auto const rcount = std::min( count, m_size - pos );
			return array_view{m_first + pos, rcount};
		}

	private:
		constexpr size_type reverse_distance( const_reverse_iterator first, const_reverse_iterator last ) const noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it with a array_view::reverse_iterator
			// will not throw.
			// return static_cast<size_type>( ( m_size - 1u ) - static_cast<size_type>( std::distance( first, last ) )
			// );
			return static_cast<size_type>( ( m_size ) - static_cast<size_type>( std::distance( first, last ) ) );
		}
	}; // array_view

	template<typename T>
	constexpr auto make_array_view( T const *first, T const *last ) noexcept {
		return array_view<T>{first, static_cast<size_t>( last - first )};
	}

	template<typename T, size_t N>
	constexpr auto make_array_view( T const ( &s )[N] ) noexcept {
		return array_view<T>{s, N};
	}

	template<typename T>
	constexpr auto make_array_view( T const *s, size_t N ) noexcept {
		return array_view<T>{s, N};
	}

	template<typename Container, std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
	constexpr auto make_array_view_impl( Container const &container ) noexcept {
		using value_t = typename std::iterator_traits<decltype( container.begin( ) )>::value_type;
		return array_view<value_t>( container.begin( ), container.size( ) );
	}

	template<typename Container>
	constexpr auto make_array_view( Container const &container ) noexcept {
		static_assert( daw::traits::is_container_like_v<Container>, "container does not satisfy requirements of a Container" );
		return make_array_view_impl( container );
	}

	struct array_view_access_past_end_exception{};

	template<typename Container, std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
	constexpr auto make_array_view( Container const &container, size_t const pos,
	                                      size_t const count = std::numeric_limits<size_t>::max( ) ) {

		using value_t = typename std::iterator_traits<decltype( container.begin( ) )>::value_type;

		if( pos >= container.size( ) ) {
			throw array_view_access_past_end_exception{};
		}
		auto const rcount = std::min( count, container.size( ) - pos );
		return array_view<value_t>{container.begin( ) + pos, rcount};
	}
} // namespace daw

namespace std {
	template<typename T>
	struct hash<daw::array_view<T>> {
		size_t operator( )( daw::array_view<T> s ) noexcept {
			auto const tot_size = sizeof( T ) + sizeof( decltype( s.size( ) ) );
			char vals[tot_size];
			*reinterpret_cast<T *>( &vals[0] ) = s.data( );
			*reinterpret_cast<decltype( s.size( ) )>( &vals[sizeof( T )] ) = s.size( );
			return daw::fnv1a_hash( vals, tot_size );
		}
	};
} // namespace std

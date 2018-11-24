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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_math.h"
#include "daw_utility.h"
#include "iterator/daw_iterator.h"
#include "iterator/daw_reverse_iterator.h"

namespace daw {
	template<typename T, typename = std::nullptr_t>
	struct span;

	template<typename T>
	struct span<T, std::enable_if_t<daw::is_const_v<std::remove_reference_t<T>>, std::nullptr_t>> {
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
		pointer m_first = nullptr;
		size_type m_size = 0;

	public:
		constexpr span( ) noexcept = default;
		explicit constexpr span( std::nullptr_t ) noexcept {}
		constexpr span( std::nullptr_t, size_type ) noexcept {}

		constexpr span( pointer ptr, size_type count ) noexcept
		  : m_first( ptr )
		  , m_size( count ) {}

		constexpr span( pointer first, pointer last ) noexcept
		  : m_first( first )
		  , m_size( last - first ) {}

		template<size_t N>
		explicit constexpr span( value_type const ( &arr )[N] ) noexcept
		  : m_first( arr )
		  , m_size( N ) {}

		template<typename Container>
		explicit constexpr span( Container const &c ) noexcept
		  : m_first( std::data( c ) )
		  , m_size( std::size( c ) ) {}

		constexpr span copy( ) const noexcept {
			return { m_first, m_size };
		}

		constexpr iterator begin( ) noexcept {
			return m_first;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr iterator end( ) noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_first + m_size;
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr reverse_iterator rend( ) noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr reference operator[]( size_type const pos ) noexcept {
			return m_first[pos];
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return m_first[pos];
		}

		constexpr reference at( size_type const pos ) {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );
			return m_first[pos];
		}

		constexpr const_reference at( size_type const pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			return m_first[pos];
		}

		constexpr reference front( ) noexcept {
			return *m_first;
		}

		constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		constexpr reference back( ) noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr const_reference back( ) const noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr pointer data( ) noexcept {
			return m_first;
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type size_bytes( ) const noexcept {
			return m_size * sizeof( value_type );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return nullptr == m_first or 0 == m_size;
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

		constexpr size_type copy( T *dest, size_type const count,
		                          size_type const pos = 0 ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			size_type const rlen = daw::min( count, m_size - pos );
			auto src = m_first + pos;
			for( size_t n = 0; n < rlen; ++n ) {
				dest[n] = src[n];
			}
			return rlen;
		}

		constexpr void fill( const_reference value ) noexcept {
			daw::algorithm::fill_n( m_first, m_size, value );
		}

		constexpr span subset(
		  size_type const pos = 0,
		  size_type const count = std::numeric_limits<size_type>::max( ) ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			auto const rcount = daw::min( count, m_size - pos );
			return span{m_first + pos, rcount};
		}

	private:
		constexpr size_type reverse_distance( const_reverse_iterator first,
		                                      const_reverse_iterator last ) const
		  noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it
			// with a span::reverse_iterator will not throw. return
			// static_cast<size_type>( ( m_size - 1u ) - static_cast<size_type>(
			// std::distance( first, last ) )
			// );
			return static_cast<size_type>(
			  ( m_size ) - static_cast<size_type>( std::distance( first, last ) ) );
		}
	};

	// T non-const span, can only copy from  copy freely
	template<typename T>
	struct span<T, std::enable_if_t<!daw::is_const_v<std::remove_reference_t<T>>, std::nullptr_t>> {
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
		pointer m_first = nullptr;
		size_type m_size = 0;

	public:
		constexpr span( ) noexcept = default;
		explicit constexpr span( std::nullptr_t ) noexcept {}
		constexpr span( std::nullptr_t, size_type ) noexcept {}

		constexpr span( span & other ) noexcept: m_first( other.m_first ), m_size( other.m_size ) { }
		constexpr span& operator=( span & rhs ) noexcept {
			m_first = rhs.m_first;
			m_size = rhs.m_size;
			return *this;
		}

		constexpr span( span && ) noexcept = default;
		constexpr span& operator=( span && ) noexcept = default;
		~span( ) = default;

		constexpr operator span<T const>( ) const noexcept {
			return { m_first, m_size };
		}

		constexpr span copy( ) const noexcept {
			return { m_first, m_size };
		}

		// Attempt to copy a const span to a non-const span
		span( span const & ) = delete;
		span & operator=( span const & ) = delete;

		constexpr span( pointer ptr, size_type count ) noexcept
		  : m_first( ptr )
		  , m_size( count ) {}

		constexpr span( pointer first, pointer last ) noexcept
		  : m_first( first )
		  , m_size( last - first ) {}

		template<size_t N>
		explicit constexpr span( value_type ( &arr )[N] ) noexcept
		  : m_first( arr )
		  , m_size( N ) {}

		template<typename Container>
		explicit constexpr span( Container &c ) noexcept
		  : m_first( std::data( c ) )
		  , m_size( std::size( c ) ) {}

		constexpr iterator begin( ) noexcept {
			return m_first;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr iterator end( ) noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_first + m_size;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_first + m_size;
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_first + m_size );
		}

		constexpr reverse_iterator rend( ) noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return daw::make_reverse_iterator( m_first );
		}

		constexpr reference operator[]( size_type const pos ) noexcept {
			return m_first[pos];
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return m_first[pos];
		}

		constexpr reference at( size_type const pos ) {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );
			return m_first[pos];
		}

		constexpr const_reference at( size_type const pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			return m_first[pos];
		}

		constexpr reference front( ) noexcept {
			return *m_first;
		}

		constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		constexpr reference back( ) noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr const_reference back( ) const noexcept {
			return *std::next( m_first, m_size - 1 );
		}

		constexpr pointer data( ) noexcept {
			return m_first;
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type size_bytes( ) const noexcept {
			return m_size * sizeof( value_type );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return nullptr == m_first or 0 == m_size;
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

		constexpr size_type copy( T *dest, size_type const count,
		                          size_type const pos = 0 ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			size_type const rlen = daw::min( count, m_size - pos );
			auto src = m_first + pos;
			for( size_t n = 0; n < rlen; ++n ) {
				dest[n] = src[n];
			}
			return rlen;
		}

		constexpr void fill( const_reference value ) noexcept {
			daw::algorithm::fill_n( m_first, m_size, value );
		}

		constexpr span subset(
		  size_type const pos = 0,
		  size_type const count = std::numeric_limits<size_type>::max( ) ) const {

			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "Attempt to access span past end" );

			auto const rcount = daw::min( count, m_size - pos );
			return span{m_first + pos, rcount};
		}

	private:
		constexpr size_type reverse_distance( const_reverse_iterator first,
		                                      const_reverse_iterator last ) const
		  noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it
			// with a span::reverse_iterator will not throw. return
			// static_cast<size_type>( ( m_size - 1u ) - static_cast<size_type>(
			// std::distance( first, last ) )
			// );
			return static_cast<size_type>(
			  ( m_size ) - static_cast<size_type>( std::distance( first, last ) ) );
		}
	};

	template<typename T>
	span( T *, size_t ) -> span<T>;

	template<typename T, size_t N>
	span( T (&)[N] ) -> span<T>;

	template<typename Container>
	span( Container & ) -> span<std::remove_reference_t<decltype( *std::data( std::declval<Container>( ) ) )>>;

	template<typename Container>
	span( Container const & ) -> span<daw::remove_cvref_t<decltype( *std::data( std::declval<Container>( ) ) )> const>;

	template<typename T>
	using view = span<T const>;
} // namespace daw

namespace std {
	template<typename T>
	struct hash<daw::span<T>> {
		size_t operator( )( daw::span<T> s ) noexcept {
			auto const tot_size = sizeof( T ) + sizeof( decltype( s.size( ) ) );
			char vals[tot_size];
			*reinterpret_cast<T *>( &vals[0] ) = s.data( );
			*reinterpret_cast<decltype( s.size( ) )>( &vals[sizeof( T )] ) =
			  s.size( );
			return daw::fnv1a_hash( vals, tot_size );
		}
	};
} // namespace std

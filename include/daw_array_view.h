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

namespace daw {
	template<typename T>
	struct array_view;

	namespace details {
		template<typename Iterator>
		constexpr std::reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
			return std::reverse_iterator<Iterator>( i );
		}
	} // namespace details

	template<class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_end( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2,
	                           ForwardIterator2 last2 ) {
		if( first2 == last2 ) {
			return last1; // specified in C++11
		}

		ForwardIterator1 ret = last1;

		while( first1 != last1 ) {
			ForwardIterator1 it1 = first1;
			ForwardIterator2 it2 = first2;
			while( *it1 == *it2 ) { // or: while (pred(*it1,*it2)) for version (2)
				++it1;
				++it2;
				if( it2 == last2 ) {
					ret = first1;
					break;
				}
				if( it1 == last1 ) {
					return ret;
				}
			}
			++first1;
		}
		return ret;
	}

	template<typename T>
	struct array_view {
		using value_type = T;
		using pointer = T *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

	  private:
		const_pointer m_first;
		size_type m_size;

	  public:
		constexpr array_view( ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr array_view( std::nullptr_t ) noexcept : m_first{nullptr}, m_size{0} {}
		constexpr array_view( std::nullptr_t, size_type ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr array_view( const_pointer s, size_type count ) noexcept : m_first{s}, m_size{count} {}

		template<size_t N>
		constexpr array_view( T const ( &s )[N] ) noexcept : array_view{s, N} {}

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

		constexpr const_iterator rbegin( ) const noexcept {
			return details::make_reverse_iterator( m_first + ( m_size - 1 ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return details::make_reverse_iterator( m_first + ( m_size - 1 ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return details::make_reverse_iterator( m_first - 1 );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return details::make_reverse_iterator( m_first - 1 );
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
			return m_first[0];
		}

		constexpr const_reference back( ) const noexcept {
			return m_first[m_size - 1];
		}

		constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		constexpr const_pointer c_str( ) const noexcept {
			return m_first;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type length( ) const noexcept {
			return m_size;
		}

		constexpr size_type max_size( ) const noexcept {
			return m_size;
		}

		constexpr bool empty( ) const noexcept {
			return 0 == m_size;
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

		constexpr int compare( array_view const v ) const noexcept {
			auto const sz = std::min( m_size, v.m_size );
			size_type count = 0;
			using std::equal_to;
			using std::less;
			for( size_type n = 0; n < sz; ++n ) {
				if( !equal_to<T>{}( m_first[n], v.m_first[n] ) ) {
					if( less<T>{}( m_first[n], v.m_first[n] ) ) {
						return -1;
					}
					return 1;
				}
			}
			if( m_size < v.m_size ) {
				return -1;
			} else if( v.m_size < m_size ) {
				return 1;
			}
			return 0;
		}

		constexpr array_view subset( size_type const pos = 0,
		                             size_type const count = std::numeric_limits<size_type>::max( ) ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access array_view past end"};
			}
			auto const rcount = std::min( count, m_size - pos );
			return array_view{m_first + pos, rcount};
		}

		constexpr int compare( size_type const pos1, size_type const count1, array_view const v ) const {
			return subset( pos1, count1 ).compare( v );
		}

		constexpr int compare( size_type const pos1, size_type const count1, array_view const v, size_type const pos2,
		                       size_type const count2 ) const {
			return subset( pos1, count1 ).compare( v.subset( pos2, count2 ) );
		}

		constexpr int compare( const_pointer s ) const noexcept {
			return compare( array_view{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s ) const {
			return subset( pos1, count1 ).compare( array_view{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s,
		                       size_type const count2 ) const {
			return subset( pos1, count1 ).compare( array_view{s, count2} );
		}

		constexpr size_type find( array_view const v, size_type const pos = 0 ) const noexcept {
			if( size( ) < v.size( ) ) {
				return size( );
			}
			if( v.empty( ) ) {
				return pos;
			}
			using std::equal_to;
			auto result = std::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), equal_to<T>{} );
			if( cend( ) == result ) {
				return size( );
			}
			return static_cast<size_type>( result - cbegin( ) );
		}

		constexpr size_type find( value_type const c, size_type const pos = 0 ) const noexcept {
			return find( array_view{&c, 1}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return find( array_view{s, count}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find( array_view{s}, pos );
		}

		constexpr size_type rfind( array_view const v,
		                           size_type const pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			if( size( ) < v.size( ) ) {
				return size( );
			}
			pos = std::min( pos, size( ) - v.size( ) );
			if( v.empty( ) ) {
				return pos;
			}
			for( auto cur = begin( ) + pos;; --cur ) {
				if( std::equal( cur, cur + v.size( ), v.begin( ) ) ) {
					return static_cast<size_type>( cur - begin( ) );
				}
				if( cur == begin( ) ) {
					return size( );
				}
			}
		}

		constexpr size_type rfind( value_type const c,
		                           size_type const pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			return rfind( array_view{&c, 1}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return rfind( array_view{s, count}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos = std::numeric_limits<size_type>::max( ) ) const
		    noexcept {
			return rfind( array_view{s}, pos );
		}

		constexpr size_type find_first_of( array_view const v, size_type const pos = 0 ) const noexcept {
			using std::equal_to;
			auto const iter = std::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), equal_to<T>{} );
			return iter == cend( ) ? m_size : static_cast<size_type>( iter - cbegin( ) );
		}

		constexpr size_type find_first_of( value_type c, size_type const pos = 0 ) const noexcept {
			return find_first_of( array_view{&c, 1}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type pos, size_type const count ) const noexcept {
			return find_first_of( array_view{s, count}, pos );
		}

		template<size_t N>
		constexpr size_t find_first_of( T const ( &s )[N], size_type const pos = 0 ) const noexcept {
			return find_first_of( array_view{s, N}, pos );
		}

	  private:
		constexpr size_type reverse_distance( const_reverse_iterator first, const_reverse_iterator last ) const
		    noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it with a array_view::reverse_iterator
			// will not throw.
			// return static_cast<size_type>( ( m_size - 1u ) - static_cast<size_type>( std::distance( first, last ) )
			// );
			return static_cast<size_type>( ( m_size ) - static_cast<size_type>( std::distance( first, last ) ) );
		}

	  public:
		constexpr size_type find_last_of( array_view s, size_type pos = std::numeric_limits<size_type>::max( ) ) const
		    noexcept {
			using std::equal_to;
			auto iter = std::find_end( cbegin( ), cend( ), s.begin( ), s.end( ), equal_to<T>{} );
			return iter == cend( ) ? m_size : static_cast<size_type>( iter - cbegin( ) );
		}

		constexpr size_type find_last_of( value_type const c,
		                                  size_type pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			for( difference_type n = m_size - 1; n >= 0; --n ) {
				if( m_first[n] == c ) {
					return static_cast<size_type>( n );
				}
			}
			return size( );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_of( array_view{s, count}, pos );
		}

		template<size_t N>
		constexpr size_type find_last_of( T const ( &s )[N],
		                                  size_type pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			return find_last_of( array_view{s, N}, pos );
		}

		constexpr size_type find_first_not_of( array_view v, size_type pos = 0 ) const noexcept {
			if( pos >= m_size ) {
				return size( );
			}
			if( 0 == v.size( ) ) {
				return pos;
			}

			const_iterator iter = find_not_of( cbegin( ) + pos, cend( ), v );
			if( cend( ) == iter ) {
				return size( );
			}

			return std::distance( cbegin( ), iter );
		}

		constexpr size_type find_first_not_of( value_type c, size_type pos = 0 ) const noexcept {
			return find_first_not_of( array_view{&c, 1}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_first_not_of( array_view{s, count}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos = 0 ) const noexcept {
			return find_first_not_of( array_view{s}, pos );
		}

		constexpr size_type find_last_not_of( array_view v,
		                                      size_type pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			if( pos >= m_size ) {
				pos = m_size - 1;
			}
			if( 0 == v.size( ) ) {
				return pos;
			}
			pos = m_size - ( pos + 1 );
			const_reverse_iterator iter = find_not_of( crbegin( ) + pos, crend( ), v );
			if( crend( ) == iter ) {
				return size( );
			}
			return reverse_distance( crbegin( ), iter );
		}

		constexpr size_type find_last_not_of( value_type c,
		                                      size_type pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			return find_last_not_of( array_view{&c, 1}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_not_of( array_view{s, count}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s,
		                                      size_type pos = std::numeric_limits<size_type>::max( ) ) const noexcept {
			return find_last_not_of( array_view{s}, pos );
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

	// array_view / array_view
	//
	template<typename T>
	constexpr bool operator==( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename T>
	constexpr bool operator!=( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename T>
	constexpr bool operator<( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename T>
	constexpr bool operator<=( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename T>
	constexpr bool operator>( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename T>
	constexpr bool operator>=( array_view<T> lhs, array_view<T> rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}
} // namespace daw

namespace std {
	template<typename T>
	struct hash<daw::array_view<T>> {
		constexpr size_t operator( )( daw::array_view<T> s ) noexcept {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};
} // namespace std

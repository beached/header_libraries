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
#include <string>

#include "daw_fnv1a_hash.h"

namespace daw {
	template<typename CharT, typename Traits = std::char_traits<CharT>>
	struct basic_string_view;

	namespace details {
		template<typename SizeT, typename CharT>
		constexpr SizeT strlen( CharT const *const str ) noexcept {
			auto pos = str;
			while( *( pos ) != 0 ) {
				++pos;
			}
			return static_cast<SizeT>( pos - str );
		}

		template<typename Iterator>
		constexpr std::reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
			return std::reverse_iterator<Iterator>( i );
		}

		template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
		constexpr InputIt
		find_first_of( InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last, BinaryPredicate p ) noexcept(
		    noexcept( std::declval<BinaryPredicate>( )( *std::declval<InputIt>( ), *std::declval<ForwardIt>( ) ) ) ) {
			for( ; first != last; ++first ) {
				for( ForwardIt it = s_first; it != s_last; ++it ) {
					if( p( *first, *it ) ) {
						return first;
					}
				}
			}
			return last;
		}

		template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
		constexpr InputIt find_first_not_of(
		    InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last,
		    BinaryPredicate
		        p ) noexcept( noexcept( std::declval<BinaryPredicate>( )( *std::declval<InputIt>( ),
		                                                                  *std::declval<ForwardIt>( ) ) ) ) {
			bool found = false;
			for( ; first != last; ++first ) {
				found = false;
				for( ForwardIt it = s_first; it != s_last; ++it ) {
					if( p( *first, *it ) ) {
						found = true;
						break;
					}
				}
				if( !found ) {
					return first;
				}
			}
			return last;
		}

		template<typename InputIt, typename Predicate>
		constexpr InputIt find_first_of_if( InputIt first, InputIt last, Predicate p ) noexcept(
		    noexcept( std::declval<Predicate>( )( *std::declval<InputIt>( ) ) ) ) {
			for( ; first != last; ++first ) {
				if( p( *first ) ) {
					return first;
				}
			}
			return last;
		}

		template<typename InputIt, typename Predicate>
		constexpr InputIt find_first_not_of_if( InputIt first, InputIt last, Predicate p ) noexcept(
		    noexcept( std::declval<Predicate>( )( *std::declval<InputIt>( ) ) ) ) {
			for( ; first != last; ++first ) {
				if( !p( *first ) ) {
					return first;
				}
			}
			return last;
		}

		template<typename charT, typename traits>
		inline void sv_insert_fill_chars( std::basic_ostream<charT, traits> &os, std::size_t n ) {
			enum { chunk_size = 8 };
			charT fill_chars[chunk_size];
			std::fill_n( fill_chars, static_cast<std::size_t>( chunk_size ), os.fill( ) );
			for( ; n >= chunk_size && os.good( ); n -= chunk_size ) {
				os.write( fill_chars, static_cast<std::size_t>( chunk_size ) );
			}
			if( n > 0 && os.good( ) ) {
				os.write( fill_chars, static_cast<std::streamsize>( n ) );
			}
		}

		template<typename charT, typename traits>
		void sv_insert_aligned( std::basic_ostream<charT, traits> &os,
		                        daw::basic_string_view<charT, traits> const &str ) {
			auto const size = str.size( );
			auto const alignment_size = static_cast<std::size_t>( os.width( ) ) - size;
			bool const align_left = ( os.flags( ) & std::basic_ostream<charT, traits>::adjustfield ) ==
			                        std::basic_ostream<charT, traits>::left;
			if( !align_left ) {
				sv_insert_fill_chars( os, alignment_size );
				if( os.good( ) ) {
					os.write( str.data( ), static_cast<std::streamsize>( size ) );
				}
			} else {
				os.write( str.data( ), static_cast<std::streamsize>( size ) );
				if( os.good( ) ) {
					sv_insert_fill_chars( os, alignment_size );
				}
			}
		}
	} // namespace details

	template<typename CharT, typename Traits>
	struct basic_string_view final {
		using traits_type = Traits;
		using value_type = CharT;
		using pointer = CharT *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type_internal = uint32_t;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

	  private:
		const_pointer m_first;
		size_type_internal m_size;

	  public:
		static constexpr size_type_internal const npos = std::numeric_limits<size_type_internal>::max( );

		constexpr basic_string_view( ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr basic_string_view( std::nullptr_t ) noexcept : m_first{nullptr}, m_size{0} {}
		constexpr basic_string_view( std::nullptr_t, size_type ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr basic_string_view( const_pointer s, size_type count ) noexcept
		    : m_first{s}, m_size{static_cast<size_type_internal>( count )} {}

		constexpr basic_string_view( basic_string_view sv, size_type count ) noexcept
		    : m_first{sv.m_first}, m_size{static_cast<size_type_internal>( count )} {}

		template<size_t N>
		constexpr basic_string_view( CharT const ( &s )[N] ) noexcept : basic_string_view{s, N} {}

		constexpr basic_string_view( const_pointer s ) noexcept
		    : basic_string_view{s, details::strlen<size_type_internal>( s )} {}

		constexpr basic_string_view( basic_string_view const &other ) noexcept = default;
		constexpr basic_string_view( basic_string_view &&other ) noexcept = default;

		constexpr basic_string_view &operator=( basic_string_view const & rhs ) noexcept {
			m_first = rhs.m_first;
			m_size = rhs.m_size;
			return *this;
		}

		constexpr basic_string_view &operator=( basic_string_view && rhs ) noexcept {
			m_first = std::move( rhs.m_first );
			m_size = std::move( rhs.m_size );
			return *this;
		}

		~basic_string_view( ) = default;

		operator std::basic_string<CharT, Traits>( ) const {
			return to_string( );
		}

		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_string_view( std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept
		    : basic_string_view{str.data( ), static_cast<size_type_internal>( str.size( ) )} {}

		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_string_view &operator=( std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept {
			m_first = str.data( );
			m_size = static_cast<size_type_internal>( str.size( ) );
			return *this;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator end( ) const noexcept {
			return &m_first[m_size];
		}

		constexpr const_iterator cend( ) const noexcept {
			return &m_first[m_size];
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return details::make_reverse_iterator( &m_first[m_size - 1] );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return details::make_reverse_iterator( &m_first[m_size - 1] );
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
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
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

		constexpr CharT pop_front( ) noexcept {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		constexpr CharT pop_back( ) noexcept {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		constexpr void resize( size_t const n ) noexcept {
			m_size = static_cast<size_type_internal>( n );
		}

		constexpr void swap( basic_string_view &v ) noexcept {
			using std::swap;
			swap( m_first, v.m_first );
			swap( m_size, v.m_size );
		}

		size_type copy( CharT *dest, size_type const count, size_type const pos = 0 ) const {
			if( pos >= m_size ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			size_type rlen = ( std::min )( count, m_size - pos );
			traits_type::copy( dest, m_first + pos, rlen );
			return rlen;
		}

		constexpr basic_string_view substr( size_type const pos = 0, size_type const count = npos ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			auto const rcount = static_cast<size_type_internal>( std::min( count, m_size - pos ) );
			return basic_string_view{&m_first[pos], rcount};
		}

		constexpr int compare( basic_string_view const v ) const noexcept {
			auto const cmp = traits_type::compare( m_first, v.m_first, ( std::min )( m_size, v.m_size ) );
			if( cmp != 0 ) {
				return cmp;
			}
			if( m_size == v.m_size ) {
				return 0;
			}
			return m_size < v.m_size ? -1 : 1;
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_string_view const v ) const {
			return substr( pos1, count1 ).compare( v );
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_string_view const v,
		                       size_type const pos2, size_type const count2 ) const {
			return substr( pos1, count1 ).compare( v.substr( pos2, count2 ) );
		}

		constexpr int compare( const_pointer s ) const noexcept {
			return compare( basic_string_view{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s ) const {
			return substr( pos1, count1 ).compare( basic_string_view{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s,
		                       size_type const count2 ) const {
			return substr( pos1, count1 ).compare( basic_string_view{s, count2} );
		}

		constexpr size_type find( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( size( ) < v.size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}
			auto result = std::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );
			if( cend( ) == result ) {
				return npos;
			}
			return static_cast<size_type_internal>( result - cbegin( ) );
		}

		constexpr size_type find( value_type const c, size_type const pos = 0 ) const noexcept {
			return find( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return find( basic_string_view{s, count}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find( basic_string_view{s}, pos );
		}

		constexpr size_type rfind( basic_string_view const v, size_type pos = npos ) const noexcept {
			if( size( ) < v.size( ) ) {
				return npos;
			}
			pos = std::min( pos, size( ) - v.size( ) );
			if( v.empty( ) ) {
				return pos;
			}
			for( auto cur = begin( ) + pos;; --cur ) {
				if( traits_type::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
					return static_cast<size_type_internal>( cur - begin( ) );
				}
				if( cur == begin( ) ) {
					return npos;
				}
			}
		}

		constexpr size_type rfind( value_type const c, size_type const pos = npos ) const noexcept {
			return rfind( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return rfind( basic_string_view{s, count}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos = npos ) const noexcept {
			return rfind( basic_string_view{s}, pos );
		}

		constexpr size_type find_first_of( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( pos >= size( ) || v.empty( ) ) {
				return npos;
			}
			auto const iter =
			    details::find_first_of( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );

			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		constexpr size_type search( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( pos + v.size( ) >= size( ) || v.empty( ) ) {
				return npos;
			}
			auto const iter = std::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.end( ), traits_type::eq );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		constexpr size_type search_last( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( pos + v.size( ) >= size( ) || v.empty( ) ) {
				return npos;
			}
			auto last_pos = pos;
			auto fpos = search( v, pos );
			while( fpos != npos ) {
				last_pos = fpos;
				fpos = search( v, fpos );
				if( fpos == last_pos ) {
					break;
				}
			}
			return last_pos;
		}

		template<typename Predicate>
		constexpr size_type find_first_of_if( Predicate pred, size_type const pos = 0 ) const
		    noexcept( noexcept( std::declval<Predicate>( )( std::declval<value_type>( ) ) ) ) {
			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter = details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type_internal>( iter - cbegin( ) );
		}

		template<typename Predicate>
		constexpr size_type find_first_not_of_if( Predicate pred, size_type const pos = 0 ) const
		    noexcept( noexcept( std::declval<Predicate>( )( std::declval<value_type>( ) ) ) ) {
			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter = details::find_first_not_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type_internal>( std::distance( cbegin( ), iter ) );
		}

		constexpr size_type find_first_of( value_type c, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type pos, size_type const count ) const noexcept {
			return find_first_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_string_view{s}, pos );
		}

		std::basic_string<value_type, traits_type> to_string( ) const {
			std::basic_string<value_type, traits_type> result;
			result.reserve( size( ) );
			std::copy_n( cbegin( ), size( ), std::back_inserter( result ) );
			return result;
		}

	  private:
		constexpr size_type reverse_distance( const_reverse_iterator first, const_reverse_iterator last ) const
		    noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it with a string_view::reverse_iterator
			// will not throw.
			return static_cast<size_type>( ( m_size - 1u ) - std::distance( first, last ) );
		}

	  public:
		constexpr size_type find_last_of( basic_string_view s, size_type pos = npos ) const noexcept {
			if( s.m_size == 0u ) {
				return npos;
			}
			if( pos >= m_size ) {
				pos = 0;
			} else {
				pos = m_size - ( pos + 1 );
			}
			const_reverse_iterator iter = std::find_first_of( crbegin( ) + static_cast<difference_type>( pos ),
			                                                  crend( ), s.crbegin( ), s.crend( ), traits_type::eq );
			return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
		}

		constexpr size_type find_last_of( value_type const c, size_type pos = npos ) const noexcept {
			for( difference_type n = m_size - 1; n >= 0; --n ) {
				if( m_first[n] == c ) {
					return static_cast<size_type>( n );
				}
			}
			return npos;
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos = npos ) const noexcept {
			return find_last_of( basic_string_view{s}, pos );
		}

		constexpr size_type find_first_not_of( basic_string_view v, size_type pos = 0 ) const noexcept {
			if( pos >= m_size ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}

			const_iterator iter = details::find_first_not_of(
			    cbegin( ) + pos, cend( ), v.cbegin( ), std::next( v.cbegin( ), static_cast<ptrdiff_t>( v.size( ) ) ),
			    traits_type::eq );
			if( cend( ) == iter ) {
				return npos;
			}

			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		constexpr size_type find_first_not_of( value_type c, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_first_not_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{s}, pos );
		}

		constexpr size_type find_last_not_of( basic_string_view v, size_type pos = npos ) const noexcept {
			if( pos >= m_size ) {
				pos = m_size - 1;
			}
			if( v.empty( ) ) {
				return pos;
			}
			pos = m_size - ( pos + 1 );
			const_reverse_iterator iter = find_not_of( crbegin( ) + pos, crend( ), v );
			if( crend( ) == iter ) {
				return npos;
			}
			return reverse_distance( crbegin( ), iter );
		}

		constexpr size_type find_last_not_of( value_type c, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_not_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{s}, pos );
		}
	}; // basic_string_view

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;

	template<typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Traits>{first, static_cast<size_t>( last - first )};
	}

	template<typename Iterator, typename CharT = std::decay_t<decltype( *std::declval<Iterator>( ) )>,
	         typename TraitsT = std::char_traits<CharT>>
	constexpr auto make_string_view_it( Iterator first, Iterator last ) noexcept {
		using sv_t = basic_string_view<CharT, TraitsT>;
		using size_type = typename sv_t::size_type;
		return sv_t{&( *first ), static_cast<size_type>( std::distance( first, last ) )};
	}
} // namespace daw

namespace std {
	template<typename CharT, typename Allocator>
	class vector;
} // namespace std

namespace daw {
	template<typename CharT, typename Allocator, typename Traits = std::char_traits<CharT>>
	constexpr auto make_string_view( std::vector<CharT, Allocator> const &v ) noexcept {
		return basic_string_view<CharT, Traits>{v.data( ), v.size( )};
	}
	// basic_string_view / basic_string_view
	//
	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// basic_string_view / basic_string
	//
	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// std::basic_string / basic_string_view
	//
	template<typename CharT, typename Traits>
	constexpr bool operator==( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( std::basic_string<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// basic_string_view / char const *
	//
	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs,
	                           typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs,
	                           typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs,
	                          typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs,
	                           typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs,
	                          typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs,
	                           typename basic_string_view<CharT, Traits>::const_pointer rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// char const * / basic_string_view
	//
	template<typename CharT, typename Traits>
	constexpr bool operator==( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                          basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                          basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( typename basic_string_view<CharT, Traits>::const_pointer lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) >= 0;
	}

	// basic_string_view / char const[N]
	//
	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs, CharT const ( &rhs )[N] ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// char const[N] / basic_string_view
	//
	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator==( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) == 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator!=( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) != 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator<( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) < 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator<=( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) <= 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator>( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) > 0;
	}

	template<typename CharT, typename Traits, size_t N>
	constexpr bool operator>=( CharT const ( &lhs )[N], basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>{lhs}.compare( rhs ) >= 0;
	}

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits> &operator<<( std::basic_ostream<CharT, Traits> &os,
	                                               daw::basic_string_view<CharT, Traits> v ) {
		if( os.good( ) ) {
			auto const size = v.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( v.data( ), static_cast<std::streamsize>( size ) );
			} else {
				details::sv_insert_aligned( os, v );
			}
			os.width( 0 );
		}
		return os;
	}

	namespace string_view_literals {
		constexpr string_view operator"" _sv( char const *str, size_t len ) noexcept {
			return daw::string_view{str, len};
		}

		constexpr u16string_view operator"" _sv( char16_t const *str, size_t len ) noexcept {
			return daw::u16string_view{str, len};
		}
		constexpr u32string_view operator"" _sv( char32_t const *str, size_t len ) noexcept {
			return daw::u32string_view{str, len};
		}

		constexpr wstring_view operator"" _sv( wchar_t const *str, size_t len ) noexcept {
			return daw::wstring_view{str, len};
		}

	} // namespace string_view_literals
} // namespace daw

template<typename CharT, typename Traits, typename Allocator>
auto operator+( std::basic_string<CharT, Traits, Allocator> lhs, daw::basic_string_view<CharT, Traits> rhs ) {
	lhs += rhs.to_string( );
	return lhs;
}

template<typename CharT, typename Traits, size_t N>
auto operator+( CharT ( &lhs )[N], daw::basic_string_view<CharT, Traits> rhs ) {
	std::basic_string<CharT, Traits> result = daw::basic_string_view<CharT, Traits>{lhs, N}.to_string( );
	result += rhs.to_string( );
	return result;
}

template<typename CharT, typename Traits>
auto operator+( CharT const *lhs, daw::basic_string_view<CharT, Traits> rhs ) {
	std::basic_string<CharT, Traits> result = daw::basic_string_view<CharT, Traits>{lhs}.to_string( );
	result += rhs.to_string( );
	return result;
}

template<typename CharT, typename Traits, typename Allocator>
auto operator+( daw::basic_string_view<CharT, Traits> lhs, std::basic_string<CharT, Traits, Allocator> const &rhs ) {
	std::basic_string<CharT, Traits, Allocator> result = lhs.to_string( ) + rhs;
	return result;
}

template<typename CharT, typename Traits, size_t N>
auto operator+( daw::basic_string_view<CharT, Traits> lhs, CharT ( &rhs )[N] ) {
	std::basic_string<CharT, Traits> result = lhs.to_string( );
	result += daw::basic_string_view<CharT, Traits>{rhs, N}.to_string( );
	return result;
}

template<typename CharT, typename Traits>
auto operator+( daw::basic_string_view<CharT, Traits> lhs, CharT const *rhs ) {
	std::basic_string<CharT, Traits> result = lhs.to_string( );
	result += daw::basic_string_view<CharT, Traits>{rhs}.to_string( );
	return result;
}

namespace std {
	// TODO use same function as string without killing performance of creating a string
	template<>
	struct hash<daw::string_view> {
		constexpr size_t operator( )( daw::string_view s ) noexcept {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};

	template<>
	struct hash<daw::wstring_view> {
		constexpr size_t operator( )( daw::wstring_view s ) noexcept {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};

	template<>
	struct hash<daw::u16string_view> {
		constexpr size_t operator( )( daw::u16string_view s ) noexcept {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};

	template<>
	struct hash<daw::u32string_view> {
		constexpr size_t operator( )( daw::u32string_view s ) noexcept {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};
} // namespace std

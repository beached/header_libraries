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
#include <limits>
#include <string>

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
		constexpr InputIt find_first_of( InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last, BinaryPredicate p ) noexcept {
			for( ; first != last; ++first ) {
				for( ForwardIt it = s_first; it != s_last; ++it ) {
					if( p( *first, *it ) ) {
						return first;
					}
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
				os.write( fill_chars, n );
			}
		}

		template<typename charT, typename traits>
		void sv_insert_aligned( std::basic_ostream<charT, traits> &os, daw::basic_string_view<charT, traits> const &str ) {
			auto const size = str.size( );
			auto const alignment_size = static_cast<std::size_t>( os.width( ) ) - size;
			bool const align_left = ( os.flags( ) & std::basic_ostream<charT, traits>::adjustfield ) ==
			                        std::basic_ostream<charT, traits>::left;
			if( !align_left ) {
				sv_insert_fill_chars( os, alignment_size );
				if( os.good( ) ) {
					os.write( str.data( ), size );
				}
			} else {
				os.write( str.data( ), size );
				if( os.good( ) ) {
					sv_insert_fill_chars( os, alignment_size );
				}
			}
		}
	} // namespace details

	template<typename CharT, typename Traits>
	struct basic_string_view {
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
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

	  private:
		const_pointer m_first;
		size_type m_size;

	  public:
		static constexpr size_type const npos = std::numeric_limits<size_type>::max( );

		constexpr basic_string_view( ) noexcept : m_first{nullptr}, m_size{0} {}

		constexpr basic_string_view( const_pointer s, size_type count ) noexcept : m_first{s}, m_size{count} {}
		constexpr basic_string_view( const_pointer s ) noexcept : m_first{s}, m_size{details::strlen<size_type>( s )} {}

		constexpr basic_string_view( basic_string_view const &other ) noexcept = default;
		constexpr basic_string_view &operator=( basic_string_view const & ) noexcept = default;
		constexpr basic_string_view &operator=( basic_string_view && ) noexcept = default;

		template<typename Allocator>
		basic_string_view( std::basic_string<value_type, traits_type, Allocator> const &str ) noexcept
		    : m_first{str.data( )}, m_size{str.size( )} {}

		template<typename Allocator>
		basic_string_view &operator=( std::basic_string<value_type, traits_type, Allocator> const &str ) noexcept {
			m_first = str.data( );
			m_size = str.size( );
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

		constexpr const_iterator rbegin( ) const noexcept {
			return details::make_reverse_iterator( &m_first[m_size-1] );
		}

		constexpr const_iterator crbegin( ) const noexcept {
			return details::make_reverse_iterator( &m_first[m_size-1] );
		}

		constexpr const_iterator rend( ) const noexcept {
			return details::make_reverse_iterator( m_first - 1 );
		}

		constexpr const_iterator crend( ) const noexcept {
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
			return m_first[m_size-1];
		}

		constexpr const_pointer data( ) const noexcept {
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

		constexpr void swap( basic_string_view &v ) noexcept {
			using std::swap;
			swap( m_first, v.m_first );
			swap( m_size, v.m_size );
		}

		constexpr size_type copy( pointer dest, size_type const count, size_type const pos = 0 ) const {
			if( pos > m_size ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			size_type const rcount = std::min( count, m_size - pos );
			for( size_type n = pos; n < rcount; ++n ) {
				*dest++ = m_first[n];
			}
			return rcount;
		}

		constexpr basic_string_view substr( size_type const pos = 0, size_type const count = npos ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			size_type const rcount = std::min( count, m_size - pos );
			return basic_string_view{&m_first[pos], rcount};
		}

		constexpr int compare( basic_string_view const v ) const noexcept {
			auto const rlen = std::min( size( ), v.size( ) );
			return traits_type::compare( data( ), v.data( ), rlen );
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
			return static_cast<size_type>( result - cbegin( ) );
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

		constexpr size_type rfind( basic_string_view const v, size_type const pos = npos ) const noexcept {
			if( size( ) < v.size( ) ) {
				return npos;
			}
			pos = std::min( pos, size( ) - v.size( ) );
			if( v.empty( ) ) {
				return pos;
			}
			for( auto cur = begin( ) + pos;; --cur ) {
				if( traits_type::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
					return static_cast<size_type>( cur - begin( ) );
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
			if( pos >= size( ) || v.size( ) == 0 ) {
				return npos;
			}
			auto const iter =
			    details::find_first_of( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );

			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( iter - cbegin( ) );
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

		std::basic_string<value_type, traits_type> to_string( ) const noexcept {
			return std::basic_string<value_type, traits_type>{ cbegin( ), size( ) };
		}
	  private:
		template<typename r_iter>
		constexpr size_type reverse_distance( const_reverse_iterator first, const_reverse_iterator last ) const noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it with a string_view::reverse_iterator
			// will not throw.
			return m_size - 1 - std::distance( first, last );
		}

	  public:
		constexpr size_type find_last_of( basic_string_view v, size_type pos = npos ) const noexcept {
			if( 0 == v.size( ) ) {
				return npos;
			}
			if( pos >= m_size ) {
				pos = 0;
			} else {
				pos = m_size - ( pos + 1 );
			}
			const_reverse_iterator iter =
			    std::find_first_of( crbegin( ) + pos, crend( ), v.cbegin( ), v.cend( ), traits_type::eq );

			if( iter == crend( ) ) {
				return npos;
			}
			return reverse_distance( crbegin( ), iter );
		}

		constexpr size_type find_last_of( CharT c, size_type pos = npos ) const noexcept {
			return find_last_of( basic_string_view{ &c, 1 }, pos );
		}

		constexpr size_type find_last_of( const CharT *s, size_type pos, size_type count ) const noexcept {
			return find_last_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_last_of( const CharT *s, size_type pos = npos ) const noexcept {
			return find_last_of( basic_string_view{s}, pos );
		}

		constexpr size_type find_first_not_of( basic_string_view v, size_type pos = 0 ) const noexcept {
			if( pos >= m_size ) {
				return npos;
			}
			if( 0 == v.size( ) ) {
				return pos;
			}

			const_iterator iter = find_not_of( cbegin( ) + pos, cend( ), v );
			if( cend( ) == iter ) {
				return npos;
			}

			return std::distance( cbegin( ), iter );
		}

		constexpr size_type find_first_not_of( CharT c, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find_first_not_of( const CharT *s, size_type pos, size_type count ) const noexcept {
			return find_first_not_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_first_not_of( const CharT *s, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{s}, pos );
		}

		constexpr size_type find_last_not_of( basic_string_view v, size_type pos = npos ) const noexcept {
			if( pos >= m_size ) {
				pos = m_size - 1;
			}
			if( 0 == v.size( ) ) {
				return pos;
			}
			pos = m_size - ( pos + 1 );
			const_reverse_iterator iter = find_not_of( crbegin( ) + pos, crend( ), v );
			if( crend( ) == iter ) {
				return npos;
			}
			return reverse_distance( crbegin( ), iter );
		}

		constexpr size_type find_last_not_of( CharT c, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{&c, 1}, pos );
		}

		constexpr size_type find_last_not_of( const CharT *s, size_type pos, size_type count ) const noexcept {
			return find_last_not_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_last_not_of( const CharT *s, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{s}, pos );
		}
	}; // basic_string_view

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;

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

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits> &operator<<( std::basic_ostream<CharT, Traits> &os,
	                                               daw::basic_string_view<CharT, Traits> v ) {
		if( os.good( ) ) {
			auto const size = v.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( v.data( ), size );
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


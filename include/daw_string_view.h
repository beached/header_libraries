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
	namespace details {
		template<typename SizeT, typename CharT>
		constexpr SizeT strlen( CharT const *const str ) noexcept {
			auto pos = str;
			while( *( pos++ ) != 0 ) {
			}
			return static_cast<SizeT>( pos - str );
		}

		template<class Iterator>
		constexpr std::reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
			return std::reverse_iterator<Iterator>( i );
		}

		template<class InputIt, class ForwardIt, class BinaryPredicate>
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
	} // namespace details

	template<typename CharT, typename Traits = std::char_traits<CharT>, typename SizeT = std::size_t>
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
		using size_type = SizeT;
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

		constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr const_iterator end( ) const noexcept {
			return begin( ) + size( );
		}

		constexpr const_iterator cend( ) const noexcept {
			return cbegin( ) + size( );
		}

		constexpr const_iterator rbegin( ) const noexcept {
			return details::make_reverse_iterator( begin( ) + size( ) - 1 );
		}

		constexpr const_iterator crbegin( ) const noexcept {
			return details::make_reverse_iterator( cbegin( ) + size( ) - 1 );
		}

		constexpr const_iterator rend( ) const noexcept {
			return details::make_reverse_iterator( begin( ) - 1 );
		}

		constexpr const_iterator crend( ) const noexcept {
			return details::make_reverse_iterator( cbegin( ) - 1 );
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return *( cbegin( ) + pos );
		}

		constexpr const_reference at( size_type const pos ) const {
			if( pos >= size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			return *( cbegin( ) + pos );
		}

		constexpr const_reference front( ) const noexcept {
			return operator[]( 0 );
		}

		constexpr const_reference back( ) const noexcept {
			return operator[]( size( ) - 1 );
		}

		constexpr const_pointer data( ) const noexcept {
			return cbegin( );
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type length( ) const noexcept {
			return m_size;
		}

		constexpr size_type max_size( ) const noexcept {
			return std::numeric_limits<size_type>::max( );
		}

		constexpr bool empty( ) const noexcept {
			return 0 == size( );
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
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			size_type const rcount = std::min( count, size( ) - pos );
			for( size_type n = pos; n < rcount; ++n ) {
				*dest++ = *( cbegin( ) + n );
			}
			return rcount;
		}

		constexpr basic_string_view substr( size_type const pos = 0, size_type const count = npos ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			size_type const rcount = std::min( count, size( ) - pos );
			return basic_string_view{cbegin( ) + pos, rcount};
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

		constexpr int compare( const_pointer s ) const {
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

		constexpr size_type find( const_pointer s, size_type const pos, size_type const count ) const {
			return find( basic_string_view{s, count}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos = 0 ) const {
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

		constexpr size_type rfind( const_pointer s, size_type const pos, size_type const count ) const {
			return rfind( basic_string_view{s, count}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos = npos ) const {
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

		constexpr size_type find_first_of( const_pointer s, size_type pos, size_type const count ) const {
			return find_first_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type const pos = 0 ) const {
			return find_first_of( basic_string_view{s}, pos );
		}

		std::basic_string<value_type, traits_type> to_string( ) const {
			return std::basic_string<value_type, traits_type>{ cbegin( ), size( ) };
		}
	}; // basic_string_view

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;

} // namespace daw


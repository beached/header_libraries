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
#include <vector>

#include "daw_algorithm.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_iterator.h"
#include "daw_math.h"
#include "daw_reverse_iterator.h"
#include "daw_string_impl.h"
#include "daw_traits.h"

namespace daw {
	template<typename T>
	constexpr size_t find_predicate_result_size( T ) noexcept {
		return 1;
	}

	template<typename CharT, typename Traits, typename InternalSizeType>
	struct basic_string_view final {
		using traits_type = Traits;
		using value_type = CharT;
		using pointer = CharT *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = daw::reverse_iterator<iterator>;
		using const_reverse_iterator = daw::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using size_type_internal = InternalSizeType;
		using difference_type = std::ptrdiff_t;

	private:
		const_pointer m_first;
		size_type_internal m_size;

		template<typename ForwardIterator>
		constexpr ForwardIterator find_not_of( ForwardIterator first, ForwardIterator const last,
		                                       basic_string_view sv ) const noexcept {
			while( first != last ) {
				auto it = sv.cbegin( );
				for( size_t n = 0; n < sv.size( ); ++n ) {
					if( *first == sv[0] ) {
						return first;
					}
				}
				++first;
			}
			return last;
		}

	public:
		static constexpr size_type_internal const npos = std::numeric_limits<size_type_internal>::max( );

		constexpr basic_string_view( ) noexcept
		  : m_first{nullptr}
		  , m_size{0} {}

		constexpr basic_string_view( std::nullptr_t ) noexcept
		  : m_first{nullptr}
		  , m_size{0} {}
		constexpr basic_string_view( std::nullptr_t, size_type ) noexcept
		  : m_first{nullptr}
		  , m_size{0} {}

		constexpr basic_string_view( const_pointer s, size_type count ) noexcept
		  : m_first{s}
		  , m_size{static_cast<size_type_internal>( count )} {}

		constexpr basic_string_view( basic_string_view sv, size_type count ) noexcept
		  : m_first{sv.m_first}
		  , m_size{static_cast<size_type_internal>( count )} {}

		template<typename Allocator>
		basic_string_view( std::basic_string<CharT, Traits, Allocator> const &str )
		  : m_first{str.data( )}
		  , m_size{str.size( )} {}

		basic_string_view( std::basic_string<CharT, Traits> const &str ) noexcept
		  : basic_string_view{str.data( ), str.size( )} {}

	private:
		/// If you really want to do this, use to_string_view as storing the address
		/// of temporaries is often a mistake
		template<typename Allocator>
		basic_string_view( std::basic_string<CharT, Traits, Allocator> &&str ) noexcept
		  : m_first{str.data( )}
		  , m_size{str.size( )} {}

		template<typename Chr, typename Tr, typename Alloc>
		friend basic_string_view<Chr, Tr> to_string_view( std::basic_string<Chr, Tr, Alloc> &&str ) noexcept;

	public:
		constexpr basic_string_view( const_pointer s ) noexcept
		  : basic_string_view{s, details::strlen<size_type_internal>( s )} {}

		constexpr basic_string_view( basic_string_view const &other ) noexcept = default;
		constexpr basic_string_view( basic_string_view &&other ) noexcept = default;

		constexpr basic_string_view &operator=( basic_string_view const &rhs ) noexcept {
			m_first = rhs.m_first;
			m_size = rhs.m_size;
			return *this;
		}

		constexpr basic_string_view &operator=( basic_string_view &&rhs ) noexcept {
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
			return daw::make_reverse_iterator( &m_first[m_size] );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return daw::make_reverse_iterator( &m_first[m_size] );
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
			return m_first == nullptr || 0 == m_size;
		}

		constexpr explicit operator bool( ) const noexcept {
			return !empty( );
		}

		constexpr void remove_prefix( size_type n ) noexcept {
#ifdef DEBUG
			if( n > m_size ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
#else
			n = daw::min( n, m_size );
#endif
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

		constexpr void clear( ) noexcept {
			m_size = 0;
		}

		constexpr CharT pop_front( ) noexcept {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		/// @brief create a substr of the first count characters and remove them from beggining
		/// @param count number of characters to remove and return
		/// @return a substr of size count starting at begin
		constexpr basic_string_view pop_front( size_t count ) noexcept {
			basic_string_view result = substr( 0, count );
			remove_prefix( count );
			return result;
		}

		/// @brief searches for where, returns substring between front and where, then pops off the substring and the where string
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		constexpr basic_string_view pop_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief creates a substr of the substr from begin to position reported true by predicate
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from beginning to position marked by predicate
		template<typename UnaryPredicate,
		         std::enable_if_t<is_unary_predicate_v<UnaryPredicate, CharT>, std::nullptr_t> = nullptr>
		constexpr basic_string_view
		pop_front( UnaryPredicate pred ) noexcept( noexcept( pred( std::declval<CharT>( ) ) ) ) {

			auto pos = find_first_of_if( std::move( pred ) );
			auto result = pop_front( pos );
			remove_prefix( find_predicate_result_size( pred ) );
			return result;
		}

		constexpr CharT pop_back( ) noexcept {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		/// @brief create a substr of the last count characters and remove them from end
		/// @param count number of characters to remove and return
		/// @return a substr of size count ending at end of string_view
		constexpr basic_string_view pop_back( size_t count ) noexcept {
			basic_string_view result = substr( size( ) - count, npos );
			remove_suffix( count );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end, then pops off the substring and the
		/// where string
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		constexpr basic_string_view pop_back( basic_string_view where ) noexcept {
			auto pos = rfind( where );
			if( pos == npos ) {
				auto result{*this};
				remove_prefix( npos );
				return result;
			}
			auto result = substr( pos + where.size( ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for last position UnaryPredicate would be true, returns substring between pred and end, then
		/// pops off the substring and the pred specified string
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from last position marked by predicate to end
		template<typename UnaryPredicate,
		         std::enable_if_t<is_unary_predicate_v<UnaryPredicate, CharT>, std::nullptr_t> = nullptr>
		constexpr basic_string_view
		pop_back( UnaryPredicate pred ) noexcept( noexcept( pred( std::declval<CharT>( ) ) ) ) {

			auto pos = find_last_of_if( std::move( pred ) );
			if( pos == npos ) {
				auto result{*this};
				remove_prefix( npos );
				return result;
			}
			auto result = substr( pos + find_predicate_result_size( pred ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, returns substring between front and where, then pops off the substring and the where
		/// string. Do nothing if where is not found
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		constexpr basic_string_view try_pop_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			if( pos == npos ) {
				return basic_string_view{};
			}
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end, then pops off the substring and the
		/// where string.  If where is not found, nothing is done
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		constexpr basic_string_view try_pop_back( basic_string_view where ) noexcept {
			auto pos = rfind( where );
			if( pos == npos ) {
				return basic_string_view{};
			}
			auto result = substr( pos + where.size( ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, and disgregards everything until the end of that
		/// @param where string to find and consume
		/// @return substring with everything up until the end of where removed
		constexpr basic_string_view & consume_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			remove_prefix( pos );
			remove_prefix( where.size( ) );
			return *this;
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
			size_type rlen = daw::min( count, m_size - pos );
			traits_type::copy( dest, m_first + pos, rlen );
			return rlen;
		}

		constexpr basic_string_view substr( size_type const pos = 0, size_type const count = npos ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_string_view past end"};
			}
			auto const rcount = static_cast<size_type_internal>( daw::min( count, m_size - pos ) );
			return basic_string_view{&m_first[pos], rcount};
		}

		static constexpr int compare( basic_string_view lhs, basic_string_view rhs ) noexcept {
			if( lhs.size( ) < rhs.size( ) ) {
				return -1;
			}
			if( lhs.size( ) > rhs.size( ) ) {
				return 1;
			}
			while( !lhs.empty( ) && !rhs.empty( ) ) {
				if( lhs.front( ) != rhs.front( ) ) {
					if( lhs.front( ) < rhs.front( ) ) {
						return -1;
					}
					return 1;
				}
				lhs.remove_prefix( );
				rhs.remove_prefix( );
			}
			return 0;
		}

		constexpr int compare( basic_string_view const rhs ) const noexcept {
			basic_string_view lhs{*this};
			return compare( lhs, rhs );
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_string_view const v ) const {
			return compare( substr( pos1, count1 ), v );
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_string_view const v,
		                       size_type const pos2, size_type const count2 ) const {
			return compare( substr( pos1, count1 ), v.substr( pos2, count2 ) );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s ) const {
			return compare( substr( pos1, count1 ), basic_string_view{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s,
		                       size_type const count2 ) const {
			return compare( substr( pos1, count1 ), basic_string_view{s, count2} );
		}

		constexpr size_type find( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( size( ) < v.size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}
			auto result = details::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );
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
			pos = daw::min( pos, size( ) - v.size( ) );
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
			auto const iter = details::find_first_of( cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );

			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		constexpr size_type search( basic_string_view const v, size_type const pos = 0 ) const noexcept {
			if( pos + v.size( ) >= size( ) || v.empty( ) ) {
				return npos;
			}
			auto const iter = details::search( cbegin( ) + pos, cend( ), v.cbegin( ), v.end( ), traits_type::eq );
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

		template<typename UnaryPredicate>
		constexpr size_type find_first_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept( std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );
			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter = details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type_internal>( iter - cbegin( ) );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_first_not_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept( std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );
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
		constexpr size_type reverse_distance( const_reverse_iterator first, const_reverse_iterator last ) const noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it with a
			// string_view::reverse_iterator will not throw.
			return ( size( ) - 1u ) - static_cast<size_t>( std::distance( first, last ) );
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
			auto iter = std::find_first_of( crbegin( ) + static_cast<difference_type>( pos ), crend( ), s.crbegin( ),
			                                s.crend( ), traits_type::eq );
			return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_last_of_if( UnaryPredicate pred, size_type pos = npos ) const noexcept {
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );
			if( pos >= m_size ) {
				pos = 0;
			} else {
				pos = m_size - ( pos + 1 );
			}
			auto iter = std::find_if( crbegin( ) + static_cast<difference_type>( pos ), crend( ), pred );
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

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N], size_type pos ) noexcept {
			return find_last_of( basic_string_view{s, N - 1}, pos );
		}

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N] ) noexcept {
			return find_last_of( basic_string_view{s, N - 1}, npos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_of( basic_string_view{s, count}, pos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos ) const noexcept {
			return find_last_of( basic_string_view{s}, pos );
		}

		constexpr size_type find_last_of( const_pointer s ) const noexcept {
			return find_last_of( basic_string_view{s}, npos );
		}

		constexpr size_type find_first_not_of( basic_string_view v, size_type pos = 0 ) const noexcept {
			if( pos >= m_size ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}

			const_iterator iter =
			  details::find_first_not_of( cbegin( ) + pos, cend( ), v.cbegin( ),
			                              std::next( v.cbegin( ), static_cast<ptrdiff_t>( v.size( ) ) ), traits_type::eq );
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
			const_reverse_iterator iter = find_not_of( crbegin( ) + static_cast<intmax_t>( pos ), crend( ), v );
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

		constexpr bool starts_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return front( ) == c;
		}

		constexpr bool starts_with( basic_string_view s ) const noexcept {
			if( s.size( ) > size( ) ) {
				return false;
			}
			auto lhs = cbegin( );
			while( !s.empty( ) ) {
				if( *lhs++ != s.front( ) ) {
					return false;
				}
				s.remove_prefix( );
			}
			return true;
		}

		constexpr bool starts_with( const_pointer s ) const noexcept {
			return starts_with( basic_string_view{s} );
		}

		constexpr bool ends_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return back( ) == c;
		}

		constexpr bool ends_with( basic_string_view s ) const noexcept {
			if( s.size( ) > size( ) ) {
				return false;
			}
			auto lhs = crbegin( );
			while( !s.empty( ) ) {
				if( *lhs++ != s.back( ) ) {
					return false;
				}
				s.remove_suffix( );
			}
			return true;
		}

		constexpr bool ends_with( const_pointer s ) const noexcept {
			return ends_with( basic_string_view{s} );
		}
	}; // basic_string_view

	template<typename Chr, typename Tr, typename Alloc>
	basic_string_view<Chr, Tr> to_string_view( std::basic_string<Chr, Tr, Alloc> &&str ) noexcept {
		return basic_string_view<Chr, Tr>{str};
	}

	template<typename CharT>
	using default_basic_string_view = basic_string_view<CharT>;

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;

	using tiny_string_view = basic_string_view<char, std::char_traits<char>, uint16_t>;
	using tiny_wstring_view = basic_string_view<wchar_t, std::char_traits<wchar_t>, uint16_t>;
	using tiny_u16string_view = basic_string_view<char16_t, std::char_traits<char16_t>, uint16_t>;
	using tiny_u32string_view = basic_string_view<char32_t, std::char_traits<char32_t>, uint16_t>;

	using small_string_view = basic_string_view<char, std::char_traits<char>, uint32_t>;
	using small_wstring_view = basic_string_view<wchar_t, std::char_traits<wchar_t>, uint32_t>;
	using small_u16string_view = basic_string_view<char16_t, std::char_traits<char16_t>, uint32_t>;
	using small_u32string_view = basic_string_view<char32_t, std::char_traits<char32_t>, uint32_t>;

	template<typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Traits>{first, static_cast<size_t>( last - first )};
	}

	template<typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_tiny_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Traits, uint16_t>{first, static_cast<uint16_t>( last - first )};
	}

	template<typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_small_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Traits, uint32_t>{first, static_cast<uint32_t>( last - first )};
	}

	template<typename Iterator, typename CharT = std::decay_t<decltype( *std::declval<Iterator>( ) )>,
	         typename TraitsT = std::char_traits<CharT>>
	constexpr auto make_string_view_it( Iterator first, Iterator last ) noexcept {
		using sv_t = basic_string_view<CharT, TraitsT>;
		using size_type = typename sv_t::size_type;
		return sv_t{&( *first ), static_cast<size_type>( std::distance( first, last ) )};
	}

	template<typename CharT, typename Allocator, typename Traits = std::char_traits<CharT>>
	auto make_string_view( std::vector<CharT, Allocator> const &v ) noexcept {
		return basic_string_view<CharT, Traits>{v.data( ), v.size( )};
	}

	template<typename CharT, typename Traits>
	daw::basic_string_view<CharT, Traits> make_string_view( std::basic_string<CharT, Traits> const &str ) {
		return daw::basic_string_view<CharT, Traits>{str};
	}

	template<typename CharT, size_t N>
	daw::basic_string_view<CharT> make_string_view( CharT const ( &str )[N] ) {
		return daw::basic_string_view<CharT>{str, N};
	}
	// basic_string_view / something else
	//
	namespace detectors {
		template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
		using can_be_string_view = decltype( daw::basic_string_view<CharT, Traits>{T{}} );
	}
	template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
	constexpr bool can_be_string_view = daw::is_detected_v<detectors::can_be_string_view, T, CharT, Traits>;

	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) == 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) == 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator==( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) == 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator==( std::basic_string<CharT, Traits> const &lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) == 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator==( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) != 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) != 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator!=( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) != 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator!=( std::basic_string<CharT, Traits> const &lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) != 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator!=( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) > 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs,
	                          std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) > 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) > 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>( std::basic_string<CharT, Traits> const &lhs,
	                          basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) > 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) >= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) >= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>=( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) >= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>=( std::basic_string<CharT, Traits> const &lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) >= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator>=( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) >= 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) < 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs,
	                          std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) < 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) < 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<( std::basic_string<CharT, Traits> const &lhs,
	                          basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) < 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) <= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) <= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<=( basic_string_view<CharT, Traits> lhs, CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, basic_string_view<CharT, Traits>{rhs} ) <= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<=( std::basic_string<CharT, Traits> const &lhs,
	                           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) <= 0;
	}
	template<typename CharT, typename Traits>
	constexpr bool operator<=( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( basic_string_view<CharT, Traits>{lhs}, rhs ) <= 0;
	}

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

	template<typename CharT, typename Traits, typename InternalSizeType, typename UnaryPredicate,
	         std::enable_if_t<daw::is_unary_predicate_v<UnaryPredicate, CharT>, std::nullptr_t> = nullptr>
	auto split( daw::basic_string_view<CharT, Traits, InternalSizeType> str, UnaryPredicate pred ) {

		class sv_arry_t {
			std::vector<daw::basic_string_view<CharT, Traits, InternalSizeType>> data;

		public:
			sv_arry_t( std::vector<daw::basic_string_view<CharT, Traits, InternalSizeType>> v )
			  : data{std::move( v )} {}
			sv_arry_t( ) = delete;
			sv_arry_t( sv_arry_t const & ) = default;
			sv_arry_t( sv_arry_t && ) = default;
			sv_arry_t &operator=( sv_arry_t const & ) = default;
			sv_arry_t &operator=( sv_arry_t && ) = default;
			~sv_arry_t( ) = default;

			decltype( auto ) operator[]( size_t p ) const noexcept {
				return data[p];
			}

			size_t size( ) const noexcept {
				return data.size( );
			}

			bool empty( ) const noexcept {
				return data.empty( );
			}

			decltype( auto ) begin( ) const noexcept {
				return data.cbegin( );
			}

			decltype( auto ) cbegin( ) const noexcept {
				return data.cbegin( );
			}

			decltype( auto ) rbegin( ) const noexcept {
				return data.crbegin( );
			}

			decltype( auto ) crbegin( ) const noexcept {
				return data.crbegin( );
			}

			decltype( auto ) end( ) const noexcept {
				return data.cend( );
			}

			decltype( auto ) cend( ) const noexcept {
				return data.cend( );
			}

			decltype( auto ) rend( ) const noexcept {
				return data.crend( );
			}

			decltype( auto ) crend( ) const noexcept {
				return data.crend( );
			}

			auto as_strings( ) const {
				std::vector<std::basic_string<CharT, Traits>> tmp{};
				tmp.reserve( size( ) );
				for( auto const &s : data ) {
					tmp.push_back( s.to_string( ) );
				}
				return tmp;
			}
		};

		std::vector<daw::basic_string_view<CharT, Traits, InternalSizeType>> v;
		auto last_pos = str.cbegin( );
		while( !str.empty( ) ) {
			auto sz = daw::min( str.size( ), str.find_first_of_if( pred ) );
			v.emplace_back( last_pos, sz );
			if( sz == str.npos ) {
				break;
			}
			str.remove_prefix( sz + 1 );
			last_pos = str.cbegin( );
		}
		v.shrink_to_fit( );
		return sv_arry_t{std::move( v )};
	}

	template<typename CharT, typename Traits, typename InternalSizeType>
	auto split( daw::basic_string_view<CharT, Traits, InternalSizeType> str, CharT const delemiter ) {
		return split( str, [delemiter]( CharT c ) noexcept { return c == delemiter; } );
	}

	template<typename CharT, typename Traits, typename InternalSizeType, size_t N>
	auto split( daw::basic_string_view<CharT, Traits, InternalSizeType> str, CharT const ( &delemiter )[N] ) {
		static_assert( N == 2, "string literal used as delemiter.  One 1 value is supported (e.g. \",\" )" );
		return split( str, [delemiter]( CharT c ) noexcept { return c == delemiter[0]; } );
	}

	template<typename CharT, typename Traits, typename Allocator, typename Delemiter>
	auto split( std::basic_string<CharT, Traits, Allocator> const &str, Delemiter d ) {
		return split( make_string_view( str ), d );
	}

	template<typename CharT, size_t N, typename Delemiter>
	auto split( CharT const ( &str )[N], Delemiter d ) {
		return split( basic_string_view<CharT>{str, N}, d );
	}

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT> &operator<<( std::basic_ostream<CharT> &os, daw::basic_string_view<CharT, Traits> v ) {
		if( os.good( ) ) {
			auto const size = v.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( v.data( ), static_cast<std::streamsize>( size ) );
			} else {
				daw::details::sv_insert_aligned( os, v );
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

	template<typename CharT, typename Traits, typename InternalSizeType>
	constexpr size_t fnv1a_hash( daw::basic_string_view<CharT, Traits, InternalSizeType> sv ) noexcept {
		return fnv1a_hash( sv.data( ), sv.size( ) );
	}

	template<size_t HashSize = sizeof( size_t ), typename CharT, typename Traits, typename InternalSizeType>
	constexpr size_t generic_hash( daw::basic_string_view<CharT, Traits, InternalSizeType> sv ) noexcept {
		return generic_hash<HashSize>( sv.data( ), sv.size( ) );
	}
} // namespace daw

namespace std {
	// TODO use same function as string without killing performance of creating a string
	template<typename CharT, typename Traits, typename InternalSizeType>
	struct hash<daw::basic_string_view<CharT, Traits, InternalSizeType>> {
		constexpr size_t operator( )( daw::basic_string_view<CharT, Traits, InternalSizeType> s ) noexcept {
			return daw::fnv1a_hash( s );
		}
	};
} // namespace std

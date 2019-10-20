// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#ifndef NOSTRING
#include <string>
#if defined( __cpp_lib_string_view )
#include <string_view>
#endif
#endif

#include <vector>

#include "daw_string_view_fwd.h"

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "impl/daw_string_impl.h"
#include "iterator/daw_iterator.h"

namespace daw {
	template<typename T>
	constexpr size_t find_predicate_result_size( T ) noexcept {
		return 1;
	}

	template<typename CharT, typename Traits, ptrdiff_t Extent>
	struct basic_string_view {
		using traits_type = Traits;
		using value_type = CharT;
		using pointer = value_type *;
		using const_pointer = std::add_const_t<value_type> *;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using const_iterator = const_pointer;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;
		static constexpr ptrdiff_t const extent = Extent;

		template<typename, typename, ptrdiff_t>
		friend struct basic_string_view;

	private:
		const_pointer m_first;
		const_pointer m_last;

		template<typename ForwardIterator>
		constexpr ForwardIterator find_not_of( ForwardIterator first,
		                                       ForwardIterator const last,
		                                       basic_string_view sv ) const
		  noexcept {
			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			while( first != last ) {
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
		static constexpr size_type const npos =
		  std::numeric_limits<size_type>::max( );

		// constructors
		constexpr basic_string_view( ) noexcept
		  : m_first( nullptr )
		  , m_last( nullptr ) {}

		constexpr basic_string_view( std::nullptr_t ) noexcept
		  : m_first( nullptr )
		  , m_last( nullptr ) {}

		constexpr basic_string_view( std::nullptr_t, size_type ) noexcept
		  : m_first( nullptr )
		  , m_last( nullptr ) {}

		constexpr basic_string_view( const_pointer s,
		                             size_type count = npos ) noexcept
		  : m_first( s )
		  , m_last(
		      std::next( s, static_cast<ptrdiff_t>(
		                      details::sstrlen<size_type>( s, count, npos ) ) ) ) {}

		constexpr basic_string_view( basic_string_view sv,
		                             size_type count ) noexcept
		  : m_first( sv.m_first )
		  , m_last( std::min( sv.size( ), count ) ) {}

		template<size_t N>
		constexpr basic_string_view( CharT const ( &cstr )[N] ) noexcept
		  : m_first( cstr )
		  , m_last( cstr + ( N - 1 ) ) {}

#ifndef NOSTRING
		template<typename Allocator>
		basic_string_view( std::basic_string<CharT, Traits, Allocator> const &str )
		  : m_first( str.data( ) )
		  , m_last( str.data( ) == nullptr ? static_cast<CharT const *>( nullptr )
		                                   : str.data( ) + str.size( ) ) {}

#if defined( __cpp_lib_string_view )
		constexpr basic_string_view(
		  std::basic_string_view<CharT, Traits> sv ) noexcept
		  : m_first( sv.data( ) )
		  , m_last( sv.data( ) + sv.size( ) ) {}
#endif
#endif

		template<ptrdiff_t Ex,
		         std::enable_if_t<( Extent == daw::dynamic_string_size and
		                            Ex != daw::dynamic_string_size ),
		                          std::nullptr_t> = nullptr>
		constexpr basic_string_view &
		operator=( basic_string_view<CharT, Traits, Ex> rhs ) noexcept {
			m_first = rhs.m_first;
			m_last = rhs.m_last;
			return *this;
		}

		constexpr basic_string_view( CharT const *first,
		                             CharT const *last ) noexcept
		  : m_first( first )
		  , m_last( last ) {}

	private:
		/// If you really want to do this, use to_string_view as storing the address
		/// of temporaries is often a mistake
#ifndef NOSTRING
		template<typename Allocator>
		basic_string_view(
		  std::basic_string<CharT, Traits, Allocator> &&str ) noexcept
		  : m_first( str.data( ) )
		  , m_last( str.data( ) + str.size( ) ) {}

		template<typename Chr, typename Tr, typename Alloc>
		friend basic_string_view<Chr, Tr>
		to_string_view( std::basic_string<Chr, Tr, Alloc> &&str ) noexcept;
#endif
		//
		// END OF constructors
	public:
		template<
		  typename Tr, ptrdiff_t Ex,
		  std::enable_if_t<( Ex == daw::dynamic_string_size and Ex != Extent ),
		                   std::nullptr_t> = nullptr>
		constexpr operator basic_string_view<CharT, Tr, Ex>( ) noexcept {
			return {m_first, m_last};
		}

#ifndef NOSTRING
		operator std::basic_string<CharT, Traits>( ) const {
			return to_string( );
		}

#if defined( __cpp_lib_string_view )
		constexpr operator std::basic_string_view<CharT, Traits>( ) const {
			return {m_first, size( )};
		}
#endif
		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_string_view(
		  std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept
		  : m_first( str.data( ) )
		  , m_last( str.data( ) + str.size( ) ) {}

		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_string_view &
		operator=( std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept {
			m_first = str.data( );
			m_last = str.data( ) + str.size( );
			return *this;
		}
#endif

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator( cend( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator( cbegin( ) );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type const pos ) const noexcept {
			return m_first[pos];
		}

		[[nodiscard]] constexpr const_reference at( size_type const pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < size( ), "Attempt to access basic_string_view past end" );
			return *std::next( m_first, static_cast<ptrdiff_t>( pos ) );
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			return *std::prev( m_last );
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_pointer c_str( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			if( m_first == nullptr ) {
				return 0U;
			}
			auto const result = distance( m_first, m_last );
			return static_cast<size_t>( result );
		}

		[[nodiscard]] constexpr size_type length( ) const noexcept {
			return static_cast<size_t>( std::distance( m_first, m_last ) );
		}

		[[nodiscard]] constexpr size_type max_size( ) const noexcept {
			return static_cast<size_t>( std::distance( m_first, m_last ) );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return size( ) == 0;
		}

		[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
			return !empty( );
		}

		constexpr void remove_prefix( size_type n ) noexcept {
			m_first =
			  std::next( m_first, static_cast<ptrdiff_t>( std::min( n, size( ) ) ) );
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( size_type n ) noexcept {
			m_last =
			  std::prev( m_last, static_cast<ptrdiff_t>( std::min( n, size( ) ) ) );
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		constexpr void clear( ) noexcept {
			m_first = nullptr;
			m_last = nullptr;
		}

		[[nodiscard]] constexpr CharT pop_front( ) noexcept {
			auto result = front( );
			remove_prefix( );
			return result;
		}

		/// @brief create a substr of the first count characters and remove them
		/// from beggining
		/// @param count number of characters to remove and return
		/// @return a substr of size count starting at begin
		[[nodiscard]] constexpr basic_string_view
		pop_front( size_t count ) noexcept {
			basic_string_view result = substr( 0, count );
			remove_prefix( count );
			return result;
		}

		/// @brief searches for where, returns substring between front and where,
		/// then pops off the substring and the where string
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		[[nodiscard]] constexpr basic_string_view
		pop_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief creates a substr of the substr from begin to position reported
		/// true by predicate
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and
		/// indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from beginning to position marked by predicate
		template<
		  typename UnaryPredicate,
		  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr basic_string_view
		pop_front( UnaryPredicate pred ) noexcept(
		  noexcept( pred( std::declval<CharT>( ) ) ) ) {

			auto pos = find_first_of_if( daw::move( pred ) );
			auto result = pop_front( pos );
			remove_prefix( find_predicate_result_size( pred ) );
			return result;
		}

		[[nodiscard]] constexpr CharT pop_back( ) noexcept {
			auto result = back( );
			remove_suffix( );
			return result;
		}

		/// @brief create a substr of the last count characters and remove them from
		/// end
		/// @param count number of characters to remove and return
		/// @return a substr of size count ending at end of string_view
		[[nodiscard]] constexpr basic_string_view
		pop_back( size_t count ) noexcept {
			basic_string_view result = substr( size( ) - count, npos );
			remove_suffix( count );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end,
		/// then pops off the substring and the where string
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		[[nodiscard]] constexpr basic_string_view
		pop_back( basic_string_view where ) noexcept {
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

		/// @brief searches for last position UnaryPredicate would be true, returns
		/// substring between pred and end, then pops off the substring and the pred
		/// specified string
		/// @tparam UnaryPredicate a unary predicate type that accepts a char and
		/// indicates with true when to stop
		/// @param pred predicate to determine where to split
		/// @return substring from last position marked by predicate to end
		template<
		  typename UnaryPredicate,
		  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr basic_string_view
		pop_back( UnaryPredicate pred ) noexcept(
		  noexcept( pred( std::declval<CharT>( ) ) ) ) {

			auto pos = find_last_of_if( daw::move( pred ) );
			if( pos == npos ) {
				auto result{*this};
				remove_prefix( npos );
				return result;
			}
			auto result = substr( pos + find_predicate_result_size( pred ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, returns substring between front and where,
		/// then pops off the substring and the where string. Do nothing if where is
		/// not found
		/// @param where string to split on and remove from front
		/// @return substring from beginning to where string
		[[nodiscard]] constexpr basic_string_view
		try_pop_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			if( pos == npos ) {
				return basic_string_view{};
			}
			auto result = pop_front( pos );
			remove_prefix( where.size( ) );
			return result;
		}

		/// @brief searches for last where, returns substring between where and end,
		/// then pops off the substring and the where string.  If where is not
		/// found, nothing is done
		/// @param where string to split on and remove from back
		/// @return substring from end of where string to end of string
		[[nodiscard]] constexpr basic_string_view
		try_pop_back( basic_string_view where ) noexcept {
			auto pos = rfind( where );
			if( pos == npos ) {
				return basic_string_view{};
			}
			auto result = substr( pos + where.size( ) );
			remove_suffix( size( ) - pos );
			return result;
		}

		/// @brief searches for where, and disgregards everything until the end of
		/// that
		/// @param where string to find and consume
		/// @return substring with everything up until the end of where removed
		[[nodiscard]] constexpr basic_string_view &
		consume_front( basic_string_view where ) noexcept {
			auto pos = find( where );
			remove_prefix( pos );
			remove_prefix( where.size( ) );
			return *this;
		}

		constexpr void resize( size_t const n ) noexcept {
			assert( n < size( ) );
			m_last = std::next( m_first, static_cast<ptrdiff_t>( n ) );
		}

		constexpr void swap( basic_string_view &v ) noexcept {
			daw::cswap( m_first, v.m_first );
			daw::cswap( m_last, v.m_last );
		}

		size_type copy( CharT *dest, size_type const count,
		                size_type const pos = 0 ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos <= size( ), "Attempt to access basic_string_view past end" );
			size_type rlen = std::min( count, size( ) - pos );
			if( rlen > 0 ) {
				traits_type::copy( dest, m_first + pos, rlen );
			}
			return rlen;
		}

		[[nodiscard]] constexpr basic_string_view
		substr( size_type const pos = 0, size_type const count = npos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos <= size( ), "Attempt to access basic_string_view past end" );
			auto const rcount =
			  static_cast<size_type>( std::min( count, size( ) - pos ) );
			return {m_first + pos, m_first + pos + rcount};
		}

	public:
		[[nodiscard]] static constexpr int
		compare( basic_string_view lhs, basic_string_view rhs ) noexcept {
			auto const str_compare = []( CharT const *p0, CharT const *p1,
			                             size_t len ) {
				auto const last = p0 + len;
				while( p0 != last ) {
					if( *p0 != *p1 ) {
						if( *p0 < *p1 ) {
							return -1;
						}
						return 1;
					}
					++p0;
					++p1;
				}
				return 0;
			};

			auto cmp = str_compare( lhs.data( ), rhs.data( ),
			                        std::min( lhs.size( ), rhs.size( ) ) );
			if( cmp == 0 ) {
				if( lhs.size( ) < rhs.size( ) ) {
					return -1;
				}
				if( rhs.size( ) < lhs.size( ) ) {
					return 1;
				}
			}
			return cmp;
		}

		[[nodiscard]] constexpr int compare( basic_string_view const rhs ) const
		  noexcept {
			basic_string_view lhs{*this};
			return compare( lhs, rhs );
		}

		constexpr int compare( size_type const pos1, size_type const count1,
		                       basic_string_view const v ) const {
			return compare( substr( pos1, count1 ), v );
		}

		[[nodiscard]] constexpr int compare( size_type const pos1,
		                                     size_type const count1,
		                                     basic_string_view const v,
		                                     size_type const pos2,
		                                     size_type const count2 ) const {
			return compare( substr( pos1, count1 ), v.substr( pos2, count2 ) );
		}

		[[nodiscard]] constexpr int compare( size_type const pos1,
		                                     size_type const count1,
		                                     const_pointer s ) const {
			return compare( substr( pos1, count1 ), basic_string_view{s} );
		}

		[[nodiscard]] constexpr int compare( size_type const pos1,
		                                     size_type const count1,
		                                     const_pointer s,
		                                     size_type const count2 ) const {
			return compare( substr( pos1, count1 ), basic_string_view{s, count2} );
		}

		[[nodiscard]] constexpr size_type find( basic_string_view const v,
		                                        size_type const pos = 0 ) const
		  noexcept {
			if( size( ) < v.size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}
			auto result = details::search( cbegin( ) + pos, cend( ), v.cbegin( ),
			                               v.cend( ), traits_type::eq );
			if( cend( ) == result ) {
				return npos;
			}
			return static_cast<size_type>( result - cbegin( ) );
		}

		[[nodiscard]] constexpr size_type find( value_type const c,
		                                        size_type const pos = 0 ) const
		  noexcept {
			return find( basic_string_view{&c, 1}, pos );
		}

		[[nodiscard]] constexpr size_type
		find( const_pointer s, size_type const pos, size_type const count ) const
		  noexcept {
			return find( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type find( const_pointer s,
		                                        size_type const pos = 0 ) const
		  noexcept {
			return find( basic_string_view{s}, pos );
		}

		[[nodiscard]] constexpr size_type rfind( basic_string_view const v,
		                                         size_type pos = npos ) const
		  noexcept {
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

		[[nodiscard]] constexpr size_type rfind( value_type const c,
		                                         size_type const pos = npos ) const
		  noexcept {
			return rfind( basic_string_view{&c, 1}, pos );
		}

		[[nodiscard]] constexpr size_type
		rfind( const_pointer s, size_type const pos, size_type const count ) const
		  noexcept {
			return rfind( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type rfind( const_pointer s,
		                                         size_type const pos = npos ) const
		  noexcept {
			return rfind( basic_string_view{s}, pos );
		}

		/// Find the first item in v that is in string
		/// \param v A range of characters to look for
		/// \param pos Starting position to start searching
		/// \return position of first item in v or npos
		[[nodiscard]] constexpr size_type
		find_first_of( basic_string_view const v, size_type const pos = 0 ) const
		  noexcept {
			if( pos >= size( ) or v.empty( ) ) {
				return npos;
			}
			auto const iter = details::find_first_of(
			  cbegin( ) + pos, cend( ), v.cbegin( ), v.cend( ), traits_type::eq );

			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type search( basic_string_view const v,
		                                          size_type const pos = 0 ) const
		  noexcept {
			if( pos + v.size( ) >= size( ) or v.empty( ) ) {
				return npos;
			}
			auto const iter = details::search( cbegin( ) + pos, cend( ), v.cbegin( ),
			                                   v.end( ), traits_type::eq );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type
		search_last( basic_string_view const v, size_type const pos = 0 ) const
		  noexcept {
			if( pos + v.size( ) >= size( ) or v.empty( ) ) {
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
		[[nodiscard]] constexpr size_type
		find_first_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept(
		    std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter =
			  details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( iter - cbegin( ) );
		}

		template<typename UnaryPredicate>
		[[nodiscard]] constexpr size_type
		find_first_not_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept(
		    std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				return npos;
			}
			auto const iter =
			  details::find_first_not_of_if( cbegin( ) + pos, cend( ), pred );
			if( cend( ) == iter ) {
				return npos;
			}
			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type
		find_first_of( value_type c, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_string_view{&c, 1}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_of( const_pointer s, size_type pos, size_type const count ) const
		  noexcept {
			return find_first_of( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_of( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_string_view{s}, pos );
		}

#ifndef NOSTRING
		[[nodiscard]] std::basic_string<value_type, traits_type>
		to_string( ) const {
			std::basic_string<value_type, traits_type> result;
			result.reserve( size( ) );
			std::copy_n( cbegin( ), size( ), std::back_inserter( result ) );
			return result;
		}
#endif
	private:
		[[nodiscard]] constexpr size_type
		reverse_distance( const_reverse_iterator first,
		                  const_reverse_iterator last ) const noexcept {
			// Portability note here: std::distance is not NOEXCEPT, but calling it
			// with a string_view::reverse_iterator will not throw.
			return ( size( ) - 1u ) -
			       static_cast<size_t>( std::distance( first, last ) );
		}

	public:
		[[nodiscard]] constexpr size_type find_last_of( basic_string_view s,
		                                                size_type pos = npos ) const
		  noexcept {
			if( s.size( ) == 0u ) {
				return npos;
			}
			if( pos >= size( ) ) {
				pos = 0;
			} else {
				pos = size( ) - ( pos + 1 );
			}
			auto iter = std::find_first_of(
			  std::next( crbegin( ), static_cast<difference_type>( pos ) ), crend( ),
			  s.crbegin( ), s.crend( ), traits_type::eq );
			return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
		}

		template<typename UnaryPredicate>
		[[nodiscard]] constexpr size_type
		find_last_of_if( UnaryPredicate pred, size_type pos = npos ) const
		  noexcept {

			traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

			if( pos >= size( ) ) {
				pos = 0;
			} else {
				pos = size( ) - ( pos + 1 );
			}
			auto iter = std::find_if(
			  crbegin( ) + static_cast<difference_type>( pos ), crend( ), pred );
			return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
		}

		[[nodiscard]] constexpr size_type find_last_of( value_type const c,
		                                                size_type pos = npos ) const
		  noexcept {
			if( pos >= size( ) ) {
				pos = 0;
			}
			auto first = std::prev( m_last );
			auto const last = std::next( m_first, pos );
			while( first != last ) {
				if( *first == 'c' ) {
					return static_cast<size_type>( std::distance( m_first, first ) );
					--first;
				}
				return npos;
			}
		}

		template<size_type N>
		[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N],
		                                                size_type pos ) noexcept {
			return find_last_of( basic_string_view{s, N - 1}, pos );
		}

		template<size_type N>
		[[nodiscard]] constexpr size_type
		find_last_of( CharT const ( &s )[N] ) noexcept {
			return find_last_of( basic_string_view{s, N - 1}, npos );
		}

		[[nodiscard]] constexpr size_type
		find_last_of( const_pointer s, size_type pos, size_type count ) const
		  noexcept {
			return find_last_of( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type find_last_of( const_pointer s,
		                                                size_type pos ) const
		  noexcept {
			return find_last_of( basic_string_view{s}, pos );
		}

		[[nodiscard]] constexpr size_type find_last_of( const_pointer s ) const
		  noexcept {
			return find_last_of( basic_string_view{s}, npos );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( basic_string_view v, size_type pos = 0 ) const noexcept {
			if( pos >= size( ) ) {
				return npos;
			}
			if( v.empty( ) ) {
				return pos;
			}

			const_iterator iter = details::find_first_not_of(
			  cbegin( ) + pos, cend( ), v.cbegin( ),
			  std::next( v.cbegin( ), static_cast<ptrdiff_t>( v.size( ) ) ),
			  traits_type::eq );
			if( cend( ) == iter ) {
				return npos;
			}

			return static_cast<size_type>( std::distance( cbegin( ), iter ) );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( value_type c, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{&c, 1}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( const_pointer s, size_type pos, size_type count ) const
		  noexcept {
			return find_first_not_of( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_first_not_of( const_pointer s, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_string_view{s}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( basic_string_view v, size_type pos = npos ) const
		  noexcept {
			if( pos >= size( ) ) {
				pos = size( ) - 1;
			}
			if( v.empty( ) ) {
				return pos;
			}
			pos = size( ) - ( pos + 1 );
			const_reverse_iterator iter =
			  find_not_of( crbegin( ) + static_cast<intmax_t>( pos ), crend( ), v );
			if( crend( ) == iter ) {
				return npos;
			}
			return reverse_distance( crbegin( ), iter );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( value_type c, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{&c, 1}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( const_pointer s, size_type pos, size_type count ) const
		  noexcept {
			return find_last_not_of( basic_string_view{s, count}, pos );
		}

		[[nodiscard]] constexpr size_type
		find_last_not_of( const_pointer s, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_string_view{s}, pos );
		}

		[[nodiscard]] constexpr bool starts_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return front( ) == c;
		}

		[[nodiscard]] constexpr bool starts_with( basic_string_view s ) const
		  noexcept {
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

		[[nodiscard]] constexpr bool starts_with( const_pointer s ) const noexcept {
			return starts_with( basic_string_view{s} );
		}

		[[nodiscard]] constexpr bool ends_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return back( ) == c;
		}

		[[nodiscard]] constexpr bool ends_with( basic_string_view s ) const
		  noexcept {
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

		[[nodiscard]] constexpr bool ends_with( const_pointer s ) const noexcept {
			return ends_with( basic_string_view{s} );
		}
	}; // basic_string_view

	// CTAD
	template<typename CharT>
	basic_string_view( CharT const *s, size_t count )
	  ->basic_string_view<CharT, std::char_traits<CharT>,
	                      daw::dynamic_string_size>;

#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator>
	basic_string_view( std::basic_string<CharT, Traits, Allocator> const &str )
	  ->basic_string_view<CharT, Traits, daw::dynamic_string_size>;

#if false && defined( __cpp_lib_string_view )
	template<typename CharT, typename Traits>
	::daw::basic_string_view( std::basic_string_view<CharT, Traits> sv )
	  ->daw::basic_string_view<CharT, Traits>;
#endif
#endif
	template<typename CharT, size_t N>
	basic_string_view( CharT const ( &cstr )[N] )
	  ->basic_string_view<CharT, std::char_traits<CharT>, N - 1>;
	//
	//
	template<typename CharT, typename Traits>
	constexpr void swap( basic_string_view<CharT, Traits> &lhs,
	                     basic_string_view<CharT, Traits> &rhs ) noexcept {

		lhs.swap( rhs );
	}

#ifndef NOSTRING
	template<typename Chr, typename Tr, typename Alloc>
	[[nodiscard]] basic_string_view<Chr, Tr>
	to_string_view( std::basic_string<Chr, Tr, Alloc> &&str ) noexcept {
		return basic_string_view<Chr, Tr>{str};
	}
#endif

	template<typename CharT, typename Traits = std::char_traits<CharT>>
	[[nodiscard]] constexpr auto
	make_string_view_it( CharT const *first, CharT const *last ) noexcept {
		return basic_string_view<CharT, Traits>{
		  first, static_cast<size_t>( last - first )};
	}

	template<
	  typename RandomIterator,
	  typename CharT = std::decay_t<decltype( *std::declval<RandomIterator>( ) )>,
	  typename TraitsT = std::char_traits<CharT>>
	[[nodiscard]] constexpr auto
	make_string_view_it( RandomIterator first, RandomIterator last ) noexcept {
		traits::is_random_access_iterator_test<RandomIterator>( );
		traits::is_input_iterator_test<RandomIterator>( );

		using sv_t = basic_string_view<CharT, TraitsT>;
		using size_type = typename sv_t::size_type;
		return sv_t{&( *first ),
		            static_cast<size_type>( std::distance( first, last ) )};
	}

	template<typename CharT, typename Allocator,
	         typename Traits = std::char_traits<CharT>>
	[[nodiscard]] auto
	make_string_view( std::vector<CharT, Allocator> const &v ) noexcept {
		return basic_string_view<CharT, Traits>{v.data( ), v.size( )};
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] daw::basic_string_view<CharT, Traits>
	make_string_view( std::basic_string<CharT, Traits> const &str ) {
		return daw::basic_string_view<CharT, Traits>{str};
	}
#endif

	template<typename CharT, size_t N>
	[[nodiscard]] constexpr daw::basic_string_view<CharT>
	make_string_view( CharT const ( &str )[N] ) {
		return daw::basic_string_view<CharT>{str, N};
	}

	template<typename CharT>
	[[nodiscard]] constexpr daw::basic_string_view<CharT>
	make_string_view( daw::basic_string_view<CharT> sv ) {
		return sv;
	}
	// basic_string_view / something else
	//
	namespace detectors {
		template<typename T, typename CharT = char,
		         typename Traits = std::char_traits<CharT>>
		using can_be_string_view =
		  decltype( daw::basic_string_view<CharT, Traits>{T{}} );
	}
	template<typename T, typename CharT = char,
	         typename Traits = std::char_traits<CharT>>
	constexpr bool can_be_string_view =
	  daw::is_detected_v<detectors::can_be_string_view, T, CharT, Traits>;

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator==( basic_string_view<CharT, Traits> lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) == 0;
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator==( basic_string_view<CharT, Traits> lhs,
	            std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) == 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator==( basic_string_view<CharT, Traits> lhs,
	                                         CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) == 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator==( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) == 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator==( CharT const *lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) == 0;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator!=( basic_string_view<CharT, Traits> lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) != 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator!=( basic_string_view<CharT, Traits> lhs,
	            std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) != 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator!=( basic_string_view<CharT, Traits> lhs,
	                                         CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) != 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator!=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) != 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator!=( CharT const *lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) != 0;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>( basic_string_view<CharT, Traits> lhs,
	           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) > 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>( basic_string_view<CharT, Traits> lhs,
	           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) > 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator>( basic_string_view<CharT, Traits> lhs,
	                                        CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) > 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>( std::basic_string<CharT, Traits> const &lhs,
	           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) > 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) > 0;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>=( basic_string_view<CharT, Traits> lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) >= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>=( basic_string_view<CharT, Traits> lhs,
	            std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) >= 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator>=( basic_string_view<CharT, Traits> lhs,
	                                         CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) >= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) >= 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator>=( CharT const *lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) >= 0;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<( basic_string_view<CharT, Traits> lhs,
	           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) < 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<( basic_string_view<CharT, Traits> lhs,
	           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) < 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator<( basic_string_view<CharT, Traits> lhs,
	                                        CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) < 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<( std::basic_string<CharT, Traits> const &lhs,
	           basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) < 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<( CharT const *lhs, basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) < 0;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<=( basic_string_view<CharT, Traits> lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare( lhs, rhs ) <= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<=( basic_string_view<CharT, Traits> lhs,
	            std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) <= 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool operator<=( basic_string_view<CharT, Traits> lhs,
	                                         CharT const *rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         lhs, basic_string_view<CharT, Traits>{rhs} ) <= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<=( std::basic_string<CharT, Traits> const &lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) <= 0;
	}
#endif
	template<typename CharT, typename Traits>
	[[nodiscard]] constexpr bool
	operator<=( CharT const *lhs,
	            basic_string_view<CharT, Traits> rhs ) noexcept {
		return basic_string_view<CharT, Traits>::compare(
		         basic_string_view<CharT, Traits>{lhs}, rhs ) <= 0;
	}
#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator>
	[[nodiscard]] auto operator+( std::basic_string<CharT, Traits, Allocator> lhs,
	                              daw::basic_string_view<CharT, Traits> rhs ) {
		lhs += rhs.to_string( );
		return lhs;
	}

	template<typename CharT, typename Traits, size_t N>
	[[nodiscard]] auto operator+( CharT ( &lhs )[N],
	                              daw::basic_string_view<CharT, Traits> rhs ) {
		std::basic_string<CharT, Traits> result =
		  daw::basic_string_view<CharT, Traits>{lhs, N}.to_string( );
		result += rhs.to_string( );
		return result;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] auto operator+( CharT const *lhs,
	                              daw::basic_string_view<CharT, Traits> rhs ) {
		std::basic_string<CharT, Traits> result =
		  daw::basic_string_view<CharT, Traits>{lhs}.to_string( );
		result += rhs.to_string( );
		return result;
	}

	template<typename CharT, typename Traits, typename Allocator>
	[[nodiscard]] auto
	operator+( daw::basic_string_view<CharT, Traits> lhs,
	           std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		std::basic_string<CharT, Traits, Allocator> result = lhs.to_string( ) + rhs;
		return result;
	}

	template<typename CharT, typename Traits, size_t N>
	[[nodiscard]] auto operator+( daw::basic_string_view<CharT, Traits> lhs,
	                              CharT ( &rhs )[N] ) {
		std::basic_string<CharT, Traits> result = lhs.to_string( );
		result += daw::basic_string_view<CharT, Traits>{rhs, N}.to_string( );
		return result;
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] auto operator+( daw::basic_string_view<CharT, Traits> lhs,
	                              CharT const *rhs ) {
		std::basic_string<CharT, Traits> result = lhs.to_string( );
		result += daw::basic_string_view<CharT, Traits>{rhs}.to_string( );
		return result;
	}
#endif

	template<typename CharT, typename Traits, typename UnaryPredicate,
	         std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Traits> str,
	                          UnaryPredicate pred ) {

		class sv_arry_t {
			std::vector<daw::basic_string_view<CharT, Traits>> data;

		public:
			sv_arry_t( std::vector<daw::basic_string_view<CharT, Traits>> v )
			  : data{daw::move( v )} {}
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

#ifndef NOSTRING
			auto as_strings( ) const {
				std::vector<std::basic_string<CharT, Traits>> tmp{};
				tmp.reserve( size( ) );
				for( auto const &s : data ) {
					tmp.push_back( s.to_string( ) );
				}
				return tmp;
			}
#endif
		};

		std::vector<daw::basic_string_view<CharT, Traits>> v;
		auto last_pos = str.cbegin( );
		while( !str.empty( ) ) {
			auto sz = std::min( str.size( ), str.find_first_of_if( pred ) );
			v.emplace_back( last_pos, sz );
			if( sz == str.npos ) {
				break;
			}
			str.remove_prefix( sz + 1 );
			last_pos = str.cbegin( );
		}
		v.shrink_to_fit( );
		return sv_arry_t{daw::move( v )};
	}

	template<typename CharT, typename Traits>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Traits> str,
	                          CharT const delemiter ) {
		return split(
		  str, [delemiter]( CharT c ) noexcept { return c == delemiter; } );
	}

	template<typename CharT, typename Traits, size_t N>
	[[nodiscard]] auto split( daw::basic_string_view<CharT, Traits> str,
	                          CharT const ( &delemiter )[N] ) {
		static_assert( N == 2,
		               "string literal used as delemiter.  One 1 value is "
		               "supported (e.g. \",\" )" );
		return split(
		  str, [delemiter]( CharT c ) noexcept { return c == delemiter[0]; } );
	}

#ifndef NOSTRING
	template<typename CharT, typename Traits, typename Allocator,
	         typename Delemiter>
	[[nodiscard]] auto
	split( std::basic_string<CharT, Traits, Allocator> const &str, Delemiter d ) {
		return split( make_string_view( str ), d );
	}
#endif
	template<typename CharT, size_t N, typename Delemiter>
	[[nodiscard]] auto split( CharT const ( &str )[N], Delemiter d ) {
		return split( basic_string_view<CharT>{str, N}, d );
	}

	template<typename CharT, typename Traits, typename OStream,
	         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os, daw::basic_string_view<CharT, Traits> v ) {
		if( os.good( ) ) {
			auto const size = v.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( v.data( ), static_cast<intmax_t>( size ) );
			} else {
				daw::details::sv_insert_aligned( os, v );
			}
			os.width( 0 );
		}
		return os;
	}

	namespace string_view_literals {
		[[nodiscard]] constexpr string_view operator"" _sv( char const *str,
		                                                    size_t len ) noexcept {
			return daw::string_view{str, len};
		}

		[[nodiscard]] constexpr u16string_view
		operator"" _sv( char16_t const *str, size_t len ) noexcept {
			return daw::u16string_view{str, len};
		}

		[[nodiscard]] constexpr u32string_view
		operator"" _sv( char32_t const *str, size_t len ) noexcept {
			return daw::u32string_view{str, len};
		}

		[[nodiscard]] constexpr wstring_view operator"" _sv( wchar_t const *str,
		                                                     size_t len ) noexcept {
			return daw::wstring_view{str, len};
		}

	} // namespace string_view_literals

	template<typename CharT, typename Traits, ptrdiff_t Extent>
	[[nodiscard]] constexpr size_t
	fnv1a_hash( daw::basic_string_view<CharT, Traits, Extent> sv ) noexcept {
		return fnv1a_hash( sv.data( ), sv.size( ) );
	}

	template<size_t HashSize = sizeof( size_t ), typename CharT, typename Traits,
	         ptrdiff_t Extent>
	[[nodiscard]] constexpr size_t
	generic_hash( daw::basic_string_view<CharT, Traits, Extent> sv ) noexcept {
		return generic_hash<HashSize>( sv.data( ), sv.size( ) );
	}

#ifndef NOSTRING
	template<typename CharT, typename... Args>
	[[nodiscard]] std::basic_string<CharT>
	to_string( daw::basic_string_view<CharT, Args...> sv ) {
		return std::basic_string<CharT>( sv.begin( ), sv.end( ) );
	}
#endif

} // namespace daw

namespace std {
	// TODO use same function as string without killing performance of creating
	// a string
	template<typename CharT, typename Traits>
	struct hash<daw::basic_string_view<CharT, Traits>> {
		[[nodiscard]] constexpr size_t
		operator( )( daw::basic_string_view<CharT, Traits> s ) noexcept {
			return daw::fnv1a_hash( s );
		}
	};
} // namespace std

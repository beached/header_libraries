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

#include "daw_container_algorithm.h"
#include "daw_fixed_stack.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_iterator.h"
#include "daw_reverse_iterator.h"
#include "daw_string_impl.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	template<typename CharT, size_t Capacity, typename Traits, typename InternalSizeType>
	struct basic_static_string final {
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
		daw::fixed_stack_t<CharT, Capacity> m_data;

	public:
		static constexpr size_type_internal const npos = std::numeric_limits<size_type_internal>::max( );

		static_assert( Capacity <= static_cast<size_t>( npos ),
		               "InternalSizeType is insufficient to represent capacity of string" );

		constexpr basic_static_string( ) noexcept : m_data{} {}

		constexpr basic_static_string( std::nullptr_t ) noexcept = delete;
		constexpr basic_static_string( std::nullptr_t, size_type ) noexcept = delete;

		constexpr basic_static_string( const_pointer s, size_type count ) : m_data{s, count} {}

		template<size_t N>
		constexpr basic_static_string( basic_static_string<CharT, N> sv, size_type count )
		  : m_data{sv.data( ), std::min( count, N )} {}

		template<typename Allocator>
		basic_static_string( std::basic_string<CharT, Traits, Allocator> const &str ) noexcept
		  : m_data{str.data( ), str.size( )} {}

		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_static_string( std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept
		  : m_data{str.data( ), static_cast<size_type_internal>( str.size( ) )} {}

		basic_static_string( std::basic_string<CharT, Traits> const &str ) noexcept : m_data{str.data( ), str.size( )} {}

		// TODO: determine if I want this or not
		// basic_static_string( std::basic_string<CharT, Traits> &&str ) noexcept = delete;

		constexpr basic_static_string( const_pointer s ) noexcept : m_data{s, details::strlen<size_type_internal>( s )} {}

		constexpr basic_static_string( basic_static_string const &other ) noexcept = default;
		constexpr basic_static_string( basic_static_string &&other ) noexcept = default;

		constexpr basic_static_string &operator=( basic_static_string const &rhs ) noexcept {
			m_data = rhs.m_data;
			return *this;
		}

		constexpr basic_static_string &operator=( basic_static_string &&rhs ) noexcept {
			m_data = std::move( rhs.m_data );
			return *this;
		}

		~basic_static_string( ) = default;

		operator std::basic_string<CharT, Traits>( ) const {
			return to_string( );
		}

		operator daw::basic_string_view<CharT, Traits>( ) const {
			return daw::basic_string_view<CharT, Traits>{m_data.data( ), m_data.size( )};
		}

		constexpr void push_back( value_type c ) {
			if( m_data.full( ) ) {
				throw std::out_of_range{"Attempt to push_back basic_static_string past end"};
			}
			m_data.push_back( c );
		}

		constexpr basic_static_string &append( value_type c ) {
			if( m_data.full( ) ) {
				throw std::out_of_range{"Attempt to append basic_static_string past end"};
			}
			m_data.push_back( c );
			return *this;
		}

		constexpr basic_static_string &append( basic_string_view<CharT, Traits> sv ) {
			if( m_data.size( ) + sv.size( ) > m_data.capacity( ) ) {
				throw std::out_of_range{"Attempt to append basic_static_string past end"};
			}
			daw::container::copy( sv, std::back_inserter( m_data ) );
			return *this;
		}

		constexpr basic_static_string &append( std::basic_string<CharT, Traits> const & str ) {
			if( m_data.size( ) + str.size( ) > m_data.capacity( ) ) {
				throw std::out_of_range{"Attempt to append basic_static_string past end"};
			}
			daw::container::copy( str, std::back_inserter( m_data ) );
			return *this;
		}

		constexpr basic_static_string &append( std::basic_string<CharT, Traits> && str ) {
			if( m_data.size( ) + str.size( ) > m_data.capacity( ) ) {
				throw std::out_of_range{"Attempt to append basic_static_string past end"};
			}
			daw::container::copy( str, std::back_inserter( m_data ) );
			return *this;
		}

		template<typename Iterator, std::enable_if_t<daw::is_iterator_v<Iterator>, std::nullptr_t> = nullptr>
		constexpr basic_static_string &append( Iterator first, Iterator const last ) {
			if( m_data.size( ) + static_cast<size_t>( daw::algorithm::distance( first, last ) ) > m_data.capacity( ) ) {
				throw std::out_of_range{"Attempt to append basic_static_string past end"};
			}
			daw::algorithm::copy( first, last, std::back_inserter( m_data ) );
			return *this;
		}

		template<typename T>
		constexpr basic_static_string &append( T const & t, size_type pos, size_type count = basic_static_string::npos ) {
			daw::basic_string_view<CharT, Traits> tmp = t;
			return append( tmp.substr( pos, count ) );
		}

		template<typename ChrT, typename TrtsT, typename Allocator>
		basic_static_string &operator=( std::basic_string<ChrT, TrtsT, Allocator> const &str ) noexcept {
			m_data = basic_static_string{str.data( ), str.size( )};
			return *this;
		}

		constexpr iterator begin( ) noexcept {
			return m_data.begin( );
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_data.begin( );
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_data.cbegin( );
		}

		constexpr iterator end( ) noexcept {
			return m_data.end( );
		}

		constexpr const_iterator end( ) const noexcept {
			return m_data.end( );
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_data.cend( );
		}

		constexpr reverse_iterator rbegin( ) noexcept {
			return daw::make_reverse_iterator( m_data.end( ) );
		}

		constexpr const_reverse_iterator rbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_data.end( ) );
		}

		constexpr const_reverse_iterator crbegin( ) const noexcept {
			return daw::make_reverse_iterator( m_data.cend( ) );
		}

		constexpr reverse_iterator rend( ) noexcept {
			return daw::make_reverse_iterator( m_data.begin( ) );
		}

		constexpr const_reverse_iterator rend( ) const noexcept {
			return daw::make_reverse_iterator( m_data.begin( ) );
		}

		constexpr const_reverse_iterator crend( ) const noexcept {
			return daw::make_reverse_iterator( m_data.cbegin( ) );
		}

		constexpr reference operator[]( size_type const pos ) noexcept {
			return m_data[pos];
		}

		constexpr const_reference operator[]( size_type const pos ) const noexcept {
			return m_data[pos];
		}

		constexpr reference at( size_type const pos ) {
			return m_data.at( pos );
		}

		constexpr const_reference at( size_type const pos ) const {
			return m_data.at( pos );
		}

		constexpr reference front( ) noexcept {
			return m_data.front( );
		}

		constexpr const_reference front( ) const noexcept {
			return m_data.front( );
		}

		constexpr reference back( ) noexcept {
			return m_data.back( );
		}

		constexpr const_reference back( ) const noexcept {
			return m_data.back( );
		}

		constexpr pointer data( ) noexcept {
			return m_data.data( );
		}

		constexpr const_pointer data( ) const noexcept {
			return m_data.data( );
		}

		constexpr const_pointer c_str( ) const noexcept {
			return m_data.data( );
		}

		constexpr size_type size( ) const noexcept {
			return m_data.size( );
		}

		constexpr size_type length( ) const noexcept {
			return m_data.size( );
		}

		constexpr size_type max_size( ) const noexcept {
			return Capacity;
		}

		constexpr bool empty( ) const noexcept {
			return m_data.empty( );
		}

		constexpr explicit operator bool( ) const noexcept {
			return !m_data.empty( );
		}

		constexpr void remove_suffix( size_type const n ) noexcept {
			m_data.resize( m_data.size( ) - n );
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		constexpr void clear( ) noexcept {
			m_data.clear( );
		}

		constexpr CharT pop_back( ) noexcept {
			auto result = m_data.back( );
			m_data.pop_back( );
			return result;
		}

		constexpr void swap( basic_static_string &v ) noexcept {
			using std::swap;
			swap( m_data, v.m_data );
		}

		constexpr size_type copy( CharT *dest, size_type const count, size_type const pos = 0 ) const {
			if( pos >= size( ) ) {
				throw std::out_of_range{"Attempt to access basic_static_string past end"};
			}
			size_type rlen = std::min( count, size( ) - pos );
			daw::algorithm::copy_n( m_data.data( ) + pos, dest, rlen );
			return rlen;
		}

		constexpr basic_static_string substr( size_type const pos = 0, size_type const count = npos ) const {
			if( pos > size( ) ) {
				throw std::out_of_range{"Attempt to access basic_static_string past end"};
			}
			auto const rcount = static_cast<size_type_internal>( std::min( count, size( ) - pos ) );
			return basic_static_string{&m_data[pos], rcount};
		}

		constexpr int compare( basic_static_string const v ) const noexcept {
			return daw::string_view{data( ), size( )}.compare( daw::string_view{v.data( ), v.size( )} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_static_string const v ) const {
			return substr( pos1, count1 ).compare( v );
		}

		constexpr int compare( size_type const pos1, size_type const count1, basic_static_string const v,
		                       size_type const pos2, size_type const count2 ) const {
			return substr( pos1, count1 ).compare( v.substr( pos2, count2 ) );
		}

		constexpr int compare( const_pointer s ) const noexcept {
			return compare( basic_static_string{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s ) const {
			return substr( pos1, count1 ).compare( basic_static_string{s} );
		}

		constexpr int compare( size_type const pos1, size_type const count1, const_pointer s,
		                       size_type const count2 ) const {
			return substr( pos1, count1 ).compare( basic_static_string{s, count2} );
		}

		constexpr size_type find( basic_static_string const v, size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.find( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find( value_type const c, size_type const pos = 0 ) const noexcept {
			return find( basic_static_string{&c, 1}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return find( basic_static_string{s, count}, pos );
		}

		constexpr size_type find( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find( basic_static_string{s}, pos );
		}

		constexpr size_type rfind( basic_static_string const v, size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.rfind( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type rfind( value_type const c, size_type const pos = npos ) const noexcept {
			return rfind( basic_static_string{&c, 1}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos, size_type const count ) const noexcept {
			return rfind( basic_static_string{s, count}, pos );
		}

		constexpr size_type rfind( const_pointer s, size_type const pos = npos ) const noexcept {
			return rfind( basic_static_string{s}, pos );
		}

		constexpr size_type find_first_of( basic_static_string const v, size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.find_first_of( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type search( basic_static_string const v, size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.search( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type search_last( basic_static_string const v, size_type const pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.search_last( daw::string_view{v.data( ), v.size( )}, pos );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_first_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept( std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );

			return daw::string_view{data( ), size( )}.find_first_of_if( pred, pos );
		}

		template<typename UnaryPredicate>
		constexpr size_type find_first_not_of_if( UnaryPredicate pred, size_type const pos = 0 ) const
		  noexcept( noexcept( std::declval<UnaryPredicate>( )( std::declval<value_type>( ) ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
			               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate" );

			return daw::string_view{data( ), size( )}.find_first_not_of_if( pred, pos );
		}

		constexpr size_type find_first_of( value_type c, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_static_string{&c, 1}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type pos, size_type const count ) const noexcept {
			return find_first_of( basic_static_string{s, count}, pos );
		}

		constexpr size_type find_first_of( const_pointer s, size_type const pos = 0 ) const noexcept {
			return find_first_of( basic_static_string{s}, pos );
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
			// static_string::reverse_iterator will not throw.
			return ( size( ) - 1u ) - static_cast<size_t>( std::distance( first, last ) );
		}

	public:
		constexpr size_type find_last_of( basic_static_string s, size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.find_last_of( daw::string_view{s.data( ), s.size( )}, pos );
		}

		constexpr size_type find_last_of( value_type const c, size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.find_last_of( c, pos );
		}

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N], size_type pos ) noexcept {
			return find_last_of( basic_static_string{s, N - 1}, pos );
		}

		template<size_type N>
		constexpr size_type find_last_of( CharT const ( &s )[N] ) noexcept {
			return find_last_of( basic_static_string{s, N - 1}, npos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_of( basic_static_string{s, count}, pos );
		}

		constexpr size_type find_last_of( const_pointer s, size_type pos ) const noexcept {
			return find_last_of( basic_static_string{s}, pos );
		}

		constexpr size_type find_last_of( const_pointer s ) const noexcept {
			return find_last_of( basic_static_string{s}, npos );
		}

		constexpr size_type find_first_not_of( basic_static_string v, size_type pos = 0 ) const noexcept {
			return daw::string_view{data( ), size( )}.find_first_not_of( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find_first_not_of( value_type c, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_static_string{&c, 1}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_first_not_of( basic_static_string{s, count}, pos );
		}

		constexpr size_type find_first_not_of( const_pointer s, size_type pos = 0 ) const noexcept {
			return find_first_not_of( basic_static_string{s}, pos );
		}

		constexpr size_type find_last_not_of( basic_static_string v, size_type pos = npos ) const noexcept {
			return daw::string_view{data( ), size( )}.find_last_not_of( daw::string_view{v.data( ), v.size( )}, pos );
		}

		constexpr size_type find_last_not_of( value_type c, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_static_string{&c, 1}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos, size_type count ) const noexcept {
			return find_last_not_of( basic_static_string{s, count}, pos );
		}

		constexpr size_type find_last_not_of( const_pointer s, size_type pos = npos ) const noexcept {
			return find_last_not_of( basic_static_string{s}, pos );
		}

		constexpr bool starts_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return front( ) == c;
		}

		constexpr bool starts_with( basic_static_string s ) const noexcept {
			return daw::string_view{data( ), size( )}.starts_with( daw::string_view{s.data( ), s.size( )} );
		}

		constexpr bool starts_with( const_pointer s ) const noexcept {
			return starts_with( basic_static_string{s} );
		}

		constexpr bool ends_with( value_type c ) const noexcept {
			if( empty( ) ) {
				return false;
			}
			return back( ) == c;
		}

		constexpr bool ends_with( basic_static_string s ) const noexcept {
			return daw::string_view{data( ), size( )}.ends_with( daw::string_view{s.data( ), s.size( )} );
		}

		constexpr bool ends_with( const_pointer s ) const noexcept {
			return ends_with( basic_static_string{s} );
		}
	}; // basic_static_string

	using static_string = basic_static_string<char, 100>;
	using wstatic_string = basic_static_string<wchar_t, 100>;
	using u16static_string = basic_static_string<char16_t, 100>;
	using u32static_string = basic_static_string<char32_t, 100>;

	using tiny_static_string = basic_static_string<char, 100, std::char_traits<char>, uint16_t>;
	using tiny_wstatic_string = basic_static_string<wchar_t, 100, std::char_traits<wchar_t>, uint16_t>;
	using tiny_u16static_string = basic_static_string<char16_t, 100, std::char_traits<char16_t>, uint16_t>;
	using tiny_u32static_string = basic_static_string<char32_t, 100, std::char_traits<char32_t>, uint16_t>;

	using small_static_string = basic_static_string<char, 100, std::char_traits<char>, uint32_t>;
	using small_wstatic_string = basic_static_string<wchar_t, 100, std::char_traits<wchar_t>, uint32_t>;
	using small_u16static_string = basic_static_string<char16_t, 100, std::char_traits<char16_t>, uint32_t>;
	using small_u32static_string = basic_static_string<char32_t, 100, std::char_traits<char32_t>, uint32_t>;

	template<size_t Capacity = 100, typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_static_string_it( CharT const *first, CharT const *last ) noexcept {
		return basic_static_string<CharT, Capacity, Traits>{first, static_cast<size_t>( last - first )};
	}

	template<size_t Capacity = 100, typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_tiny_static_string_it( CharT const *first, CharT const *last ) noexcept {
		return basic_static_string<CharT, Capacity, Traits, uint16_t>{first, static_cast<uint16_t>( last - first )};
	}

	template<size_t Capacity = 100, typename CharT, typename Traits = std::char_traits<CharT>>
	constexpr auto make_small_static_string_it( CharT const *first, CharT const *last ) noexcept {
		return basic_static_string<CharT, Capacity, Traits, uint32_t>{first, static_cast<uint32_t>( last - first )};
	}

	template<size_t Capacity = 100, typename Iterator,
	         typename CharT = std::decay_t<decltype( *std::declval<Iterator>( ) )>,
	         typename TraitsT = std::char_traits<CharT>>
	constexpr auto make_static_string_it( Iterator first, Iterator last ) noexcept {
		using sv_t = basic_static_string<CharT, Capacity, TraitsT>;
		using size_type = typename sv_t::size_type;
		return sv_t{&( *first ), static_cast<size_type>( std::distance( first, last ) )};
	}

	template<size_t Capacity = 100, typename CharT, typename Allocator, typename Traits = std::char_traits<CharT>>
	auto make_static_string( std::vector<CharT, Allocator> const &v ) noexcept {
		return basic_static_string<CharT, Capacity, Traits>{v.data( ), v.size( )};
	}

	template<size_t Capacity = 100, typename CharT, typename Traits>
	daw::basic_static_string<CharT, Capacity, Traits> make_static_string( std::basic_string<CharT, Traits> const &str ) {
		return daw::basic_static_string<CharT, Capacity, Traits>{str};
	}

	template<size_t Capacity = 100, typename CharT, size_t N>
	daw::basic_static_string<CharT, Capacity> make_static_string( CharT const ( &str )[N] ) {
		return daw::basic_static_string<CharT, Capacity>{str, N};
	}

	// basic_static_string / basic_static_string
	//
	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator==( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                           basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator!=( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                           basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator<( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                          basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator<=( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                           basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator>( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                          basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t CapacityL, size_t CapacityR, typename Traits>
	constexpr bool operator>=( basic_static_string<CharT, CapacityL, Traits> const &lhs,
	                           basic_static_string<CharT, CapacityR, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) >= 0;
	}

	// basic_static_string / basic_string_view
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           daw::basic_string_view<CharT, Traits> rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) >= 0;
	}

	// daw::basic_string_view / daw::basic_static_string
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( daw::basic_string_view<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( daw::basic_string_view<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( daw::basic_string_view<CharT, Traits> const &lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( daw::basic_string_view<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( daw::basic_string_view<CharT, Traits> const &lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( daw::basic_string_view<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// basic_static_string / basic_string
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           std::basic_string<CharT, Traits> const &rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	// std::basic_string / daw::basic_static_string
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( std::basic_string<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( std::basic_string<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( std::basic_string<CharT, Traits> const &lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( std::basic_string<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( std::basic_string<CharT, Traits> const &lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( std::basic_string<CharT, Traits> const &lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) >= 0;
	}

	// daw::basic_static_string / char const *
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( daw::basic_static_string<CharT, Capacity, Traits> lhs,
	                           typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) >= 0;
	}

	// char const * / daw::basic_static_string
	//
	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator==( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator!=( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator<=( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	constexpr bool operator>=( typename daw::basic_static_string<CharT, Capacity, Traits>::const_pointer lhs,
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) >= 0;
	}

	// daw::basic_static_string / char const[N]
	//
	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator==( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator!=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator<( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator<=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator>( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                          CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator>=( daw::basic_static_string<CharT, Capacity, Traits> const &lhs,
	                           CharT const ( &rhs )[N] ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs.data( ), lhs.size( )}.compare( rhs ) >= 0;
	}

	// char const[N] / daw::basic_static_string
	//
	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator==( CharT const ( &lhs )[N],
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) == 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator!=( CharT const ( &lhs )[N],
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) != 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator<( CharT const ( &lhs )[N],
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) < 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator<=( CharT const ( &lhs )[N],
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) <= 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator>( CharT const ( &lhs )[N],
	                          daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) > 0;
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	constexpr bool operator>=( CharT const ( &lhs )[N],
	                           daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) noexcept {
		return daw::basic_string_view<CharT, Traits>{lhs}.compare( rhs ) >= 0;
	}

	/// Appending
	template<typename CharT, size_t Capacity, typename Traits, typename Allocator>
	auto operator+( std::basic_string<CharT, Traits, Allocator> lhs,
	                daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) {
		lhs.append( rhs.cbegin( ), rhs.cend( ) );
		return std::move( lhs );
	}

	template<typename CharT, typename Traits, size_t N, size_t Capacity>
	auto operator+( CharT ( &lhs )[N], daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) {
		static_assert( N > Capacity, "Not enough space to perform append" );
		daw::basic_static_string<CharT, Capacity, Traits> result{lhs, N};
		result.append( rhs.cbegin( ), rhs.cend( ) );
		return result;
	}

	template<typename CharT, size_t Capacity, typename Traits>
	auto operator+( CharT const *lhs, daw::basic_static_string<CharT, Capacity, Traits> const &rhs ) {
		daw::basic_static_string<CharT, Capacity, Traits> result{lhs};
		result.append( rhs.cbegin( ), rhs.cend( ) );
		return result;
	}

	template<typename CharT, size_t Capacity, typename Traits, typename Allocator>
	auto operator+( daw::basic_static_string<CharT, Capacity, Traits> lhs,
	                std::basic_string<CharT, Traits, Allocator> const &rhs ) {
		lhs.append( rhs );
		return std::move( lhs );
	}

	template<typename CharT, size_t Capacity, typename Traits, typename Allocator>
	auto operator+( daw::basic_static_string<CharT, Capacity, Traits> lhs,
	                std::basic_string<CharT, Traits, Allocator> &&rhs ) {
		lhs.append( std::move( rhs ) );
		return std::move( lhs );
	}

	template<typename CharT, size_t Capacity, typename Traits, size_t N>
	auto operator+( daw::basic_static_string<CharT, Capacity, Traits> lhs, CharT ( &rhs )[N] ) {
		static_assert( Capacity < ( N - 1 ), "Not enough space to perform append" );
		lhs.append( daw::basic_string_view<CharT, Traits>{ rhs } );
		return std::move( lhs );
	}

	template<typename CharT, size_t Capacity, typename Traits>
	auto operator+( daw::basic_static_string<CharT, Capacity, Traits> lhs, CharT const *rhs ) {
		lhs.append( daw::basic_string_view<CharT, Traits>{rhs} );
		return std::move( lhs );
	}

	template<typename CharT, size_t N, typename Traits, typename InternalSizeType, typename UnaryPredicate>
	auto split( daw::basic_static_string<CharT, N, Traits, InternalSizeType> const &str, UnaryPredicate pred ) {
		static_assert( daw::is_unary_predicate_v<UnaryPredicate, CharT>,
		               "UnaryPredicate p does not fullfill the requires of a unary predicate concept.  See "
		               "http://en.cppreference.com/w/cpp/concept/Predicate" );

		return split( daw::string_view{str.data( ), str.size( )}, std::move( pred ) );
	}

	template<typename CharT, size_t Capacity, typename Traits, typename InternalSizeType>
	auto split( daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType> const &str, CharT const delemiter ) {
		return split( str, [delemiter]( CharT c ) noexcept { return c == delemiter; } );
	}

	template<typename CharT, size_t Capacity, typename Traits, typename InternalSizeType, size_t N>
	auto split( daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType> const &str,
	            CharT const ( &delemiter )[N] ) {
		static_assert( N == 2, "string literal used as delemiter.  One 1 value is supported (e.g. \",\" )" );
		return split( str, [delemiter]( CharT c ) noexcept { return c == delemiter[0]; } );
	}

	template<typename CharT, size_t Capacity, typename Traits>
	std::basic_ostream<CharT> &operator<<( std::basic_ostream<CharT> &os,
	                                       daw::basic_static_string<CharT, Capacity, Traits> const &str ) {
		if( os.good( ) ) {
			auto const size = str.size( );
			auto const w = static_cast<std::size_t>( os.width( ) );
			if( w <= size ) {
				os.write( str.data( ), static_cast<std::streamsize>( size ) );
			} else {
				daw::details::sv_insert_aligned( os, str );
			}
			os.width( 0 );
		}
		return os;
	}

	template<typename CharT, size_t Capacity, typename Traits, typename InternalSizeType>
	constexpr size_t
	fnv1a_hash( daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType> const &str ) noexcept {
		return fnv1a_hash( str.data( ), str.size( ) );
	}

	template<size_t HashSize = sizeof( size_t ), typename CharT, size_t Capacity, typename Traits, typename InternalSizeType>
	constexpr size_t generic_hash( daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType> const &str ) noexcept {
		return generic_hash<HashSize>( str.data( ), str.size( ) );
	}

	namespace detectors {
		template<typename String>
		using can_be_static_string = decltype( daw::static_string{std::declval<String>( )} );
	}

	template<typename String>
	constexpr bool can_be_static_string = daw::is_detected_v<detectors::can_be_static_string, String>;
} // namespace daw

namespace std {
	// TODO use same function as string without killing performance of creating a string
	template<typename CharT, size_t Capacity, typename Traits, typename InternalSizeType>
	struct hash<daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType>> {
		constexpr size_t
		operator( )( daw::basic_static_string<CharT, Capacity, Traits, InternalSizeType> const &str ) noexcept {
			return daw::fnv1a_hash( str );
		}
	};
} // namespace std


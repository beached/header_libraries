// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_concepts.h"
#include "daw_consteval.h"
#include "daw_utility.h"
#include "impl/daw_view_tags.h"
#include "wrap_iter.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

namespace daw {
	template<typename, bool ExplicitConv = false>
	struct contiguous_view;

	template<typename T, bool ExplicitConv>
	requires( not std::is_const_v<T> ) struct contiguous_view<T, ExplicitConv> {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using iterator = wrap_iter<pointer, contiguous_view>;
		using const_iterator = wrap_iter<const_pointer, contiguous_view>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	private:
		pointer m_first = nullptr;
		pointer m_last = nullptr;

	public:
		contiguous_view( ) = default;
		constexpr contiguous_view( contiguous_view &other ) noexcept
		  : m_first( other.m_first )
		  , m_last( other.m_last ) {}

		constexpr contiguous_view &operator=( contiguous_view &rhs ) noexcept {
			m_first = rhs.m_first;
			m_last = rhs.m_last;
			return *this;
		}

		constexpr contiguous_view( pointer first, pointer last ) noexcept
		  : m_first( first )
		  , m_last( last ) {}

		constexpr contiguous_view( pointer first, size_type sz ) noexcept
		  : m_first( first )
		  , m_last( first + sz ) {}

		template<ContiguousContainerOf<value_type> Container>
		requires( not_cvref_of<contiguous_view, Container> ) explicit(
		  ExplicitConv ) constexpr contiguous_view( Container &&c ) noexcept
		  : m_first( std::data( c ) )
		  , m_last( daw::data_end( c ) ) {}

		template<constructible_from<pointer, size_type> Container>
		explicit constexpr operator Container( ) const noexcept {
			return Container{ data( ), size( ) };
		}

		constexpr contiguous_view
		subspan( std::size_t offset, std::size_t count = std::size_t( -1 ) ) const {
			auto sz = std::min( count, size( ) - std::min( size( ), offset ) );
			return contiguous_view( data( ) + offset, data( ) + sz );
		}

		/// This is potentially unsafe.  Decrements the start of range
		void add_prefix( ) noexcept {
			--m_first;
		}

		/// This is potentially unsafe.  Decrements the start of range
		void add_prefix( std::size_t n ) noexcept {
			m_first -= n;
		}

		/// This is potentially unsafe.  Decrements the start of range
		void add_suffix( ) noexcept {
			++m_last;
		}

		/// This is potentially unsafe.  Decrements the start of range
		void add_suffix( std::size_t n ) noexcept {
			m_last += n;
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr pointer data_end( ) noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_pointer data_end( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return iterator{ m_first };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return const_iterator{ m_first };
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return const_iterator{ m_first };
		}

		[[nodiscard]] constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return iterator{ m_last };
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return const_iterator{ m_last };
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return const_iterator{ m_last };
		}

		[[nodiscard]] constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return static_cast<size_type>( m_last - m_first );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_first == m_last;
		}

		[[nodiscard]] constexpr reference operator[]( size_type index ) noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= index );
			return *( m_first + index );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type index ) const noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= index );
			return *( m_first + index );
		}

		[[nodiscard]] constexpr reference front( ) noexcept {
			assert( not empty( ) );
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			assert( not empty( ) );
			return *m_first;
		}

		[[nodiscard]] constexpr reference back( ) noexcept {
			assert( not empty( ) );
			return *( m_last - 1 );
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			assert( not empty( ) );
			return *( m_last - 1 );
		}

	private:
		[[noreturn]] void throw_out_of_range( ) const {
			::daw::exception::throw_out_of_range( "contiguous_view" );
		}

	public:
		[[nodiscard]] constexpr reference at( size_type index ) {
			if( DAW_UNLIKELY( index >= size( ) ) ) {
				throw_out_of_range( );
			}
			return *( m_first + index );
		}

		[[nodiscard]] constexpr const_reference at( size_type index ) const {
			if( DAW_UNLIKELY( index >= size( ) ) ) {
				throw_out_of_range( );
			}
			return *( m_first + index );
		}

		constexpr void remove_prefix( std::size_t count ) noexcept {
			count = ( std::min )( count, size( ) );
			m_first += count;
		}

		constexpr void remove_prefix( std::size_t count,
		                              dont_clip_to_bounds_t ) noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= count );
			m_first += count;
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( std::size_t count ) noexcept {
			count = ( std::min )( count, size( ) );
			m_last -= count;
		}

		constexpr void remove_suffix( std::size_t count,
		                              dont_clip_to_bounds_t ) noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= count );
			m_last -= count;
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		[[nodiscard]] constexpr value_type pop_front( ) noexcept
		  requires( std::is_trivially_copyable_v<value_type> ) {
			assert( not empty( ) );
			return *m_first++;
		}

		[[nodiscard]] constexpr value_type pop_back( ) noexcept
		  requires( std::is_trivially_copyable_v<value_type> ) {
			assert( not empty( ) );
			return *( --m_last );
		}

	private:
		[[nodiscard]] constexpr pointer
		find_raw( const_reference needle ) noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos != needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr const_pointer
		find_raw( const_reference needle ) const noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos != needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr pointer
		find_first_not_of_raw( const_reference needle ) noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos == needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr const_pointer
		find_first_not_of_raw( const_reference needle ) const noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos == needle ) {
				++pos;
			}
			return pos;
		}

	public:
		[[nodiscard]] constexpr iterator find( const_reference needle ) noexcept {
			return iterator{ find_raw( needle ) };
		}

		[[nodiscard]] constexpr const_iterator
		find( const_reference needle ) const noexcept {
			return const_iterator{ find_raw( needle ) };
		}

		[[nodiscard]] constexpr iterator
		find_first_not_of( const_reference needle ) noexcept {
			return iterator{ find_first_not_of_raw( needle ) };
		}

		[[nodiscard]] constexpr const_iterator
		find_first_not_of( const_reference needle ) const noexcept {
			return const_iterator{ find_first_not_of_raw( needle ) };
		}

		constexpr contiguous_view &remove_prefix_until( const_reference needle,
		                                                discard_t ) noexcept {
			auto pos = find_raw( needle );
			remove_prefix( ( pos - m_first ) + 1 );
			return *this;
		}

		constexpr contiguous_view &
		remove_prefix_until( const_reference needle ) noexcept {

			auto pos = find_raw( needle );
			remove_prefix( pos - m_first, dont_clip_to_bounds );
			return *this;
		}

		constexpr contiguous_view &remove_prefix_while( const_reference needle,
		                                                discard_t ) noexcept {
			auto pos = find_first_not_of_raw( needle );
			remove_prefix( ( pos - m_first ) + 1 );
			return *this;
		}

		constexpr contiguous_view &
		remove_prefix_while( const_reference needle ) noexcept {
			auto pos = find_first_not_of_raw( needle );
			remove_prefix( pos - m_first, dont_clip_to_bounds );
			return *this;
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_until( const_reference needle ) noexcept {
			auto const f = m_first;
			remove_prefix_until( needle );
			return contiguous_view( f, m_first );
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_until( const_reference needle, discard_t ) noexcept {
			auto const f = m_first;
			remove_prefix_until( needle );
			auto result = contiguous_view( f, m_first );
			remove_prefix( );
			return result;
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_while( const_reference needle ) noexcept {
			auto const f = m_first;
			remove_prefix_while( needle );
			return contiguous_view( f, m_first );
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_while( const_reference needle, discard_t ) noexcept {
			auto const f = m_first;
			remove_prefix_while( needle );
			auto result = contiguous_view( f, m_first );
			remove_prefix( );
			return result;
		}

		[[nodiscard]] friend constexpr bool operator==( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			size_type const sz = x.size( );
			return sz == y.size( ) and
			       std::equal( x.data( ), x.data_end( ), y.data( ) );
		}

		[[nodiscard]] friend constexpr bool operator!=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( x == y );
		}

		[[nodiscard]] friend constexpr bool operator<( contiguous_view const &x,
		                                               contiguous_view const &y ) {
			return std::lexicographical_compare( x.data( ), x.data_end( ), y.data( ),
			                                     y.data_end( ) );
		}

		[[nodiscard]] friend constexpr bool operator>( contiguous_view const &x,
		                                               contiguous_view const &y ) {
			return y < x;
		}

		[[nodiscard]] friend constexpr bool operator>=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( x < y );
		}

		[[nodiscard]] friend constexpr bool operator<=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( y < x );
		}
	};

	template<typename T, bool ExplicitConv>
	requires( std::is_const_v<T> ) struct contiguous_view<T, ExplicitConv> {

		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using iterator = wrap_iter<pointer, contiguous_view>;
		using const_iterator = wrap_iter<const_pointer, contiguous_view>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	private:
		pointer m_first = nullptr;
		pointer m_last = nullptr;

	public:
		contiguous_view( ) = default;

		contiguous_view(
		  contiguous_view<std::remove_const_t<T>> const &other ) noexcept
		  : m_first( other.data( ) )
		  , m_last( other.data_end( ) ) {}

		constexpr contiguous_view( pointer first, pointer last ) noexcept
		  : m_first( first )
		  , m_last( last ) {}

		constexpr contiguous_view( pointer first, size_type sz ) noexcept
		  : m_first( first )
		  , m_last( first + sz ) {}

		template<ContiguousContainerOf<value_type> Container>
		requires( not_cvref_of<contiguous_view, Container> ) explicit(
		  ExplicitConv ) constexpr contiguous_view( Container &&c ) noexcept
		  : m_first( std::data( c ) )
		  , m_last( daw::data_end( c ) ) {}

		template<constructible_from<pointer, size_type> Container>
		explicit constexpr operator Container( ) const noexcept {
			return Container{ data( ), size( ) };
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr pointer data_end( ) noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_pointer data_end( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return iterator{ m_first };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return const_iterator{ m_first };
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return const_iterator{ m_first };
		}

		[[nodiscard]] constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator{ end( ) };
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return iterator{ m_last };
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return const_iterator{ m_last };
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return const_iterator{ m_last };
		}

		[[nodiscard]] constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator{ begin( ) };
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return static_cast<size_type>( m_last - m_first );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_first == m_last;
		}

		[[nodiscard]] constexpr reference operator[]( size_type index ) noexcept {
			assert( index < size( ) );
			return *( m_first + index );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type index ) const noexcept {
			assert( index < size( ) );
			return *( m_first + index );
		}

	private:
		[[noreturn]] void throw_out_of_range( ) const {
			::daw::exception::throw_out_of_range( "contiguous_view" );
		}

	public:
		[[nodiscard]] constexpr reference at( size_type index ) {
			if( DAW_UNLIKELY( index >= size( ) ) ) {
				throw_out_of_range( );
			}
			return *( m_first + index );
		}

		[[nodiscard]] constexpr const_reference at( size_type index ) const {
			if( DAW_UNLIKELY( index >= size( ) ) ) {
				throw_out_of_range( );
			}
			return *( m_first + index );
		}

		constexpr void remove_prefix( std::size_t count ) noexcept {
			count = ( std::min )( count, size( ) );
			m_first += count;
		}

		constexpr void remove_prefix( std::size_t count,
		                              dont_clip_to_bounds_t ) noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= count );
			m_first += count;
		}

		constexpr void remove_prefix( ) noexcept {
			remove_prefix( 1 );
		}

		constexpr void remove_suffix( std::size_t count ) noexcept {
			count = ( std::min )( count, size( ) );
			m_last -= count;
		}

		constexpr void remove_suffix( std::size_t count,
		                              dont_clip_to_bounds_t ) noexcept {
			assert( static_cast<size_type>( m_last - m_first ) >= count );
			m_last -= count;
		}

		constexpr void remove_suffix( ) noexcept {
			remove_suffix( 1 );
		}

		[[nodiscard]] constexpr value_type pop_front( ) noexcept
		  requires( std::is_trivially_copyable_v<value_type> ) {
			assert( not empty( ) );
			return *m_first++;
		}

		[[nodiscard]] constexpr value_type pop_back( ) noexcept
		  requires( std::is_trivially_copyable_v<value_type> ) {
			assert( not empty( ) );
			return *( --m_last );
		}

	private:
		[[nodiscard]] constexpr pointer
		find_raw( const_reference needle ) noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos != needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr const_pointer
		find_raw( const_reference needle ) const noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos != needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr pointer
		find_first_not_of_raw( const_reference needle ) noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos == needle ) {
				++pos;
			}
			return pos;
		}

		[[nodiscard]] constexpr const_pointer
		find_first_not_of_raw( const_reference needle ) const noexcept {
			auto pos = m_first;
			while( pos != m_last and *pos == needle ) {
				++pos;
			}
			return pos;
		}

	public:
		[[nodiscard]] constexpr iterator find( const_reference needle ) noexcept {
			return iterator{ find_raw( needle ) };
		}

		[[nodiscard]] constexpr const_iterator
		find( const_reference needle ) const noexcept {
			return const_iterator{ find_raw( needle ) };
		}

		[[nodiscard]] constexpr iterator
		find_first_not_of( const_reference needle ) noexcept {
			return iterator{ find_first_not_of_raw( needle ) };
		}

		[[nodiscard]] constexpr const_iterator
		find_first_not_of( const_reference needle ) const noexcept {
			return const_iterator{ find_first_not_of_raw( needle ) };
		}

		constexpr contiguous_view &remove_prefix_until( const_reference needle,
		                                                discard_t ) noexcept {
			auto pos = find_raw( needle );
			remove_prefix( ( pos - m_first ) + 1 );
			return *this;
		}

		constexpr contiguous_view &
		remove_prefix_until( const_reference needle ) noexcept {

			auto pos = find_raw( needle );
			remove_prefix( pos - m_first, dont_clip_to_bounds );
			return *this;
		}

		constexpr contiguous_view &remove_prefix_while( const_reference needle,
		                                                discard_t ) noexcept {
			auto pos = find_first_not_of_raw( needle );
			remove_prefix( ( pos - m_first ) + 1 );
			return *this;
		}

		constexpr contiguous_view &
		remove_prefix_while( const_reference needle ) noexcept {
			auto pos = find_first_not_of_raw( needle );
			remove_prefix( pos - m_first, dont_clip_to_bounds );
			return *this;
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_until( const_reference needle ) noexcept {
			auto const f = m_first;
			remove_prefix_until( needle );
			return contiguous_view( f, m_first );
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_until( const_reference needle, discard_t ) noexcept {
			auto const f = m_first;
			remove_prefix_until( needle );
			auto result = contiguous_view( f, m_first );
			remove_prefix( );
			return result;
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_while( const_reference needle ) noexcept {
			auto const f = m_first;
			remove_prefix_while( needle );
			return contiguous_view( f, m_first );
		}

		[[nodiscard]] constexpr contiguous_view
		pop_front_while( const_reference needle, discard_t ) noexcept {
			auto const f = m_first;
			remove_prefix_while( needle );
			auto result = contiguous_view( f, m_first );
			remove_prefix( );
			return result;
		}

		[[nodiscard]] friend constexpr bool operator==( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			size_type const sz = x.size( );
			return sz == y.size( ) and
			       std::equal( x.data( ), x.data_end( ), y.data( ) );
		}

		[[nodiscard]] friend constexpr bool operator!=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( x == y );
		}

		[[nodiscard]] friend constexpr bool operator<( contiguous_view const &x,
		                                               contiguous_view const &y ) {
			return std::lexicographical_compare( x.data( ), x.data_end( ), y.data( ),
			                                     y.data_end( ) );
		}

		[[nodiscard]] friend constexpr bool operator>( contiguous_view const &x,
		                                               contiguous_view const &y ) {
			return y < x;
		}

		[[nodiscard]] friend constexpr bool operator>=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( x < y );
		}

		[[nodiscard]] friend constexpr bool operator<=( contiguous_view const &x,
		                                                contiguous_view const &y ) {
			return not( y < x );
		}
	};

	template<typename T>
	contiguous_view( T *, T * ) -> contiguous_view<T>;

	template<typename T>
	contiguous_view( T *, std::size_t ) -> contiguous_view<T>;

	template<ContiguousContainer Container>
	contiguous_view( Container &&c )
	  -> contiguous_view<std::remove_reference_t<decltype( *std::data( c ) )>>;

	template<typename T, std::size_t N>
	contiguous_view( T ( & )[N] ) -> contiguous_view<T>;

} // namespace daw

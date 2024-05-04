// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_bounded_array.h"
#include "daw_construct_a.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_swap.h"

#include <array>
#include <iterator>

namespace daw {
	template<typename T, std::size_t N>
	struct bounded_vector_t {
		static_assert( std::is_default_constructible_v<T>,
		               "T must be default constructible" );
		static_assert( std::is_trivially_destructible_v<T>,
		               "T must be trivially destructible" );
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator = value_type *;
		using const_iterator = value_type const *;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

	private:
		std::size_t m_index = 0;
		std::size_t m_first = 0;
		bounded_array_t<T, N> m_stack{ };

	public:
		bounded_vector_t( )= default;

		constexpr bounded_vector_t( const_pointer ptr, size_type count ) noexcept
		  : m_index{ ( daw::min )( count, N ) } {

			daw::algorithm::copy_n( ptr, m_stack.begin( ), ( daw::min )( count, N ) );
		}

		template<typename Iterator>
		constexpr bounded_vector_t( Iterator first, Iterator last ) {
			auto out = daw::algorithm::copy( first, last, m_stack.begin( ) );
			m_index =
			  static_cast<std::size_t>( std::distance( m_stack.begin( ), out ) );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_index == m_first;
		}

		[[nodiscard]] constexpr bool full( ) const noexcept {
			return m_index - m_first == N;
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_index - m_first;
		}

		[[nodiscard]] static constexpr size_type capacity( ) noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool has_room( size_type count ) noexcept {
			return count + size( ) >= N;
		}

		[[nodiscard]] constexpr size_type available( ) const noexcept {
			return N - size( );
		}

		constexpr void clear( ) noexcept {
			m_index = 0;
			m_first = 0;
		}

		[[nodiscard]] constexpr reference front( ) noexcept {
			return m_stack[m_first];
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			return m_stack[m_first];
		}

		[[nodiscard]] constexpr reference back( ) noexcept {
			return m_stack[m_index - 1];
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			return m_stack[m_index - 1];
		}

		[[nodiscard]] constexpr reference operator[]( size_type pos ) noexcept {
			return m_stack[m_first + pos];
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type pos ) const noexcept {
			return m_stack[m_first + pos];
		}

		[[nodiscard]] constexpr reference at( size_type pos ) {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		[[nodiscard]] constexpr const_reference at( size_type pos ) const {
			daw::exception::precondition_check(
			  pos < size( ), "Attempt to access past end of fix_stack" );
			return m_stack[pos + m_first];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_stack.data( ) + m_first;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_stack.data( ) + m_first;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return m_stack.begin( ) + m_first;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_stack.begin( ) + m_first;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_stack.cbegin( ) + m_first;
		}

		[[nodiscard]] constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator( cend( ) );
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return &m_stack[m_index];
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return &m_stack[m_index];
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return &m_stack[m_index];
		}

		[[nodiscard]] constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator( cbegin( ) );
		}

	private:
		constexpr void do_move_to_front( ) noexcept {
			for( std::size_t n = m_first; n < m_index; ++n ) {
				m_stack[n - m_first] = m_stack[n];
			}
			m_index -= m_first;
			m_first = 0;
		}

		[[nodiscard]] constexpr bool can_move_front( size_type how_many ) noexcept {
			return m_first > 0 and m_index >= N - how_many;
		}

	public:
		constexpr void push_back( const_reference value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index] = value;
			++m_index;
		}

		constexpr void push_back( value_type &&value ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			m_stack[m_index] = std::move( value );
			++m_index;
		}

		/**
		 * This gets around things like std::pair not having constexpr operator=
		 * @return position of pushed value
		 */
		constexpr std::size_t push_back( ) noexcept {
			if( can_move_front( 1 ) ) {
				do_move_to_front( );
			}
			return m_index++;
		}

		constexpr void push_back( const_pointer ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( std::size_t n = start; n < m_index; ++n ) {
				m_stack[n] = *ptr;
				++ptr;
			}
		}

		constexpr void assign( size_type count, const_reference value ) noexcept {
			clear( );
			for( std::size_t n = 0; n < count; ++n ) {
				push_back( value );
			}
		}

		template<typename Ptr>
		constexpr void push_back( Ptr const *ptr, size_type sz ) noexcept {
			if( can_move_front( sz ) ) {
				do_move_to_front( );
			}
			auto const start = m_index;
			m_index += sz;
			for( std::size_t n = start; n < m_index; ++n ) {
				m_stack[n] = static_cast<value_type>( *ptr );
				++ptr;
			}
		}

		template<typename... Args>
		constexpr void emplace_back( Args &&...args ) noexcept {
			if( can_move_front( sizeof...( Args ) ) ) {
				do_move_to_front( );
			}
			m_stack[m_index] =
			  daw::construct_a<value_type>( DAW_FWD( args )... );
			++m_index;
		}

		[[nodiscard]] constexpr value_type pop_back( ) noexcept {
			return m_stack[--m_index];
		}

		constexpr void pop_front( size_type const count ) {
			daw::exception::precondition_check<std::out_of_range>(
			  m_index <= m_index - count, "Attempt to pop_front past end of stack" );

			m_index += count;
		}

		///	take care calling as it is slow
		[[nodiscard]] constexpr value_type pop_front( ) {
			auto result = front( );
			pop_front( 1 );
			return result;
		}

		constexpr void resize( size_type const count ) {

			daw::exception::precondition_check<std::out_of_range>(
			  count <= capacity( ), "Attempt to resize past capacity of fix_stack" );

			if( count > size( ) ) {
				if( can_move_front( count ) ) {
					do_move_to_front( );
				}
				for( size_type n = size( ); n < count; ++n ) {
					m_stack[n] = value_type{ };
				}
			}
			m_index = count;
		}

		constexpr iterator erase( const_iterator pos ) {
			auto first = std::next( begin( ), std::distance( cbegin( ), pos ) );
			auto last = std::next( first, 1 );
			while( last != end( ) ) {
				daw::iter_swap( first, last );
				std::advance( first, 1 );
				std::advance( last, 1 );
			}
			resize( size( ) - 1 );
			if( pos == cend( ) ) {
				return end( );
			}
			return std::next( begin( ), std::distance( cbegin( ), pos ) + 1 );
		}

		constexpr iterator erase( const_iterator first, const_iterator last ) {
			auto first1 = std::next( begin( ), std::distance( cbegin( ), first ) );
			auto first2 = std::next( begin( ), std::distance( cbegin( ), last ) );
			while( first2 != end( ) ) {
				daw::iter_swap( first1, first2 );
				std::advance( first1, 1 );
				std::advance( first2, 1 );
			}
			auto const dist = std::distance( first, last );

			daw::exception::dbg_precondition_check<std::out_of_range>(
			  static_cast<difference_type>( size( ) ) > dist and dist >= 0,
			  "Attempt to resize past capacity of fix_stack" );

			resize( size( ) - static_cast<size_type>( dist ) );
			if( last == cend( ) ) {
				return end( );
			}
			return std::next( begin( ), std::distance( cbegin( ), last ) + 1 );
		}

		constexpr void zero( ) noexcept {
			for( std::size_t n = 0; n < N; ++n ) {
				m_stack[n] = 0;
			}
		}

		constexpr void swap( bounded_vector_t &rhs ) {
			daw::cswap( m_index, rhs.m_index );
			daw::cswap( m_first, rhs.m_first );
			for( std::size_t n = 0; n < size( ); ++n ) {
				daw::cswap( operator[]( n ), rhs[n] );
			}
		}
	};

	template<typename T>
	struct bounded_vector_t<T, 0> {
		static_assert( std::is_default_constructible_v<T>,
		               "T must be default constructible" );
		static_assert( std::is_trivially_destructible_v<T>,
		               "T must be trivially destructible" );
		using value_type = T;
		using reference = void;
		using const_reference = void;
		using iterator = void;
		using const_iterator = T const *;
		using reverse_iterator = std::reverse_iterator<T *>;
		using const_reverse_iterator = std::reverse_iterator<T const *>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		bounded_vector_t( ) = default;
		constexpr bounded_vector_t( const_pointer, size_type ) noexcept {}

		template<typename Iterator>
		constexpr bounded_vector_t( Iterator, Iterator ) {
			std::terminate( );
		}

		static constexpr bool empty( ) noexcept {
			return true;
		}

		static constexpr bool full( ) noexcept {
			return true;
		}

		static constexpr size_type size( ) noexcept {
			return 0;
		}

		static constexpr size_type capacity( ) noexcept {
			return 0;
		}

		static constexpr bool has_room( size_type /*count*/ ) noexcept {
			return false;
		}

		static constexpr size_type available( ) noexcept {
			return 0;
		}

		static constexpr void clear( ) noexcept {}

		static constexpr const_reference front( ) noexcept {
			std::terminate( );
		}

		static constexpr const_reference back( ) noexcept {
			std::terminate( );
		}

		constexpr const_reference operator[]( size_type ) noexcept {
			std::terminate( );
		}

		static constexpr const_reference at( size_type /*pos*/ ) {
			std::terminate( );
		}

		static constexpr pointer data( ) noexcept {
			return nullptr;
		}

		static constexpr const_iterator begin( ) noexcept {
			return nullptr;
		}

		static constexpr const_iterator cbegin( ) noexcept {
			return nullptr;
		}

		static constexpr const_reverse_iterator rbegin( ) noexcept {
			return const_reverse_iterator( end( ) );
		}

		static constexpr const_reverse_iterator crbegin( ) noexcept {
			return const_reverse_iterator( cend( ) );
		}

		static constexpr const_iterator end( ) noexcept {
			return nullptr;
		}

		static constexpr const_iterator cend( ) noexcept {
			return nullptr;
		}

		static constexpr const_reverse_iterator rend( ) noexcept {
			return const_reverse_iterator( begin( ) );
		}

		static constexpr const_reverse_iterator crend( ) noexcept {
			return const_reverse_iterator( cbegin( ) );
		}

		static constexpr void resize( size_type const count ) {
			daw::exception::precondition_check<std::out_of_range>(
			  count <= capacity( ), "Attempt to resize past capacity of fix_stack" );
		}

		static constexpr void zero( ) noexcept {}

		static constexpr void swap( bounded_vector_t & /*rhs*/ ) {}
	};
} // namespace daw

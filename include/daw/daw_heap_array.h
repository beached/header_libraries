// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_check_exceptions.h"
#include "daw_exception.h"
#include "daw_exchange.h"
#include "daw_swap.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace daw {
	template<typename T>
	struct heap_array {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;

	private:
		value_type *m_begin = nullptr;
		value_type *m_end = nullptr;
		size_t m_size = 0;

		static value_type *create_value( size_t n ) noexcept {
#if defined( DAW_USE_EXCEPTIONS )
			try {
#endif
				return new value_type[n];
#if defined( DAW_USE_EXCEPTIONS )
			} catch( std::bad_alloc const & ) { std::terminate( ); } catch( ... ) {
				return nullptr;
			}
#endif
		}

	public:
		heap_array( ) = default;

		heap_array( size_t Size )
		  : m_begin( create_value( Size + 1 ) )
		  , m_end( m_begin + Size )
		  , m_size( Size ) {}

		heap_array( size_t Size, value_type const &def_value )
		  : heap_array( Size ) {

			std::fill( m_begin, m_end, def_value );
		}

		constexpr heap_array( heap_array &&other ) noexcept
		  : m_begin( daw::exchange( other.m_begin, nullptr ) )
		  , m_end( daw::exchange( other.m_begin, nullptr ) )
		  , m_size( daw::exchange( other.m_size, 0ULL ) ) {}

		constexpr heap_array &operator=( heap_array &&rhs ) noexcept {
			m_begin = daw::exchange( rhs.m_begin, nullptr );
			m_end = daw::exchange( rhs.m_end, nullptr );
			m_size = daw::exchange( rhs.m_size, 0ULL );
			return *this;
		}

		heap_array( heap_array const &other )
		  : m_begin( create_value( other.m_size ) )
		  , m_end( m_begin + other.m_size )
		  , m_size( other.m_size ) {

			std::copy_n( other.m_begin, m_size, m_begin );
		}

		heap_array &operator=( heap_array const &rhs ) {
			T *tmp = create_value( rhs.m_size );
			T *tmp_end = std::copy_n( rhs.m_begin, rhs.m_size, tmp );
			std::size_t tmp_sz = rhs.m_size;
			clear( );
			m_begin = tmp;
			m_end = tmp_end;
			m_size = tmp_sz;
			return *this;
		}

		heap_array &operator=( std::initializer_list<value_type> const &values ) {
			heap_array tmp( values.size( ) );
			std::copy_n( values.begin( ), values.size( ), tmp.m_begin );
			daw::cswap( *this, tmp );
			return *this;
		}

		heap_array( T *arry, size_t Size )
		  : m_begin( create_value( Size ) )
		  , m_end( m_begin + Size )
		  , m_size( Size ) {

			std::copy_n( arry, Size, m_begin );
		}

		constexpr void clear( ) noexcept {
			auto tmp = daw::exchange( m_begin, nullptr );
			m_size = 0;
			m_end = nullptr;
			delete[] tmp;
		}

		constexpr void swap( heap_array &rhs ) noexcept {
			daw::cswap( m_begin, rhs.m_begin );
			daw::cswap( m_end, rhs.m_end );
			daw::cswap( m_size, rhs.m_size );
		}

		~heap_array( ) {
			clear( );
		}

		explicit constexpr operator bool( ) const noexcept {
			return nullptr == m_begin;
		}

		constexpr size_t size( ) const noexcept {
			return m_size;
		}

		constexpr bool empty( ) const noexcept {
			return m_begin == m_end;
		}

		constexpr iterator data( ) noexcept {
			return m_begin;
		}

		constexpr const_iterator data( ) const noexcept {
			return m_begin;
		}

		constexpr iterator begin( ) noexcept {
			return m_begin;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_begin;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_begin;
		}

		constexpr iterator end( ) noexcept {
			return m_end;
		}

		constexpr const_iterator end( ) const noexcept {
			return m_end;
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_end;
		}

		constexpr reference operator[]( size_t pos ) {
			return *( m_begin + pos );
		}

		constexpr const_reference operator[]( size_t pos ) const {
			return *( m_begin + pos );
		}

		constexpr reference at( size_t pos ) {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "position is beyond end of heap_array" );

			return operator[]( pos );
		}

		constexpr const_reference at( size_t pos ) const {
			daw::exception::precondition_check<std::out_of_range>(
			  pos < m_size, "position is beyond end of heap_array" );

			return operator[]( pos );
		}

		constexpr reference front( ) {
			return *m_begin;
		}

		constexpr const_reference front( ) const {
			return *m_begin;
		}

		constexpr reference back( ) {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		constexpr const_reference back( ) const {
			auto tmp = m_end;
			--tmp;
			return *tmp;
		}

		constexpr iterator find_first_of( const_reference value,
		                                  const_iterator start_at ) const {
			assert( m_begin != nullptr );
			assert( start_at != nullptr );
			*m_end = value;
			if( start_at != nullptr ) {
				while( *start_at != value ) {
					++start_at;
				}
			}
			return const_cast<iterator>( start_at );
		}

		iterator find_first_of( const_reference value ) const {
			return find_first_of( value, cbegin( ) );
		}
	}; // struct heap_array

	template<typename T>
	constexpr void swap( daw::heap_array<T> &lhs,
	                     daw::heap_array<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}
} // namespace daw

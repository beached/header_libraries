// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_exchange.h"
#include "daw_swap.h"

#include <ciso646>
#include <memory>
#include <stdexcept>

namespace daw {
	template<typename T>
	struct unique_array_t {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = size_t;
		using iterator = value_type *;
		using const_iterator = value_type const *;

	private:
		pointer m_data = nullptr;
		size_type m_size = 0;

		static pointer create_values( size_t n ) noexcept {
			try {
				return new value_type[n];
			} catch( std::bad_alloc const & ) { std::abort( ); }
		}

		constexpr void clear( ) {
			auto tmp = daw::exchange( m_data, nullptr );
			m_size = 0;
			if( tmp ) {
				delete[] tmp;
			}
		}

	public:
		constexpr unique_array_t( ) noexcept = default;
		constexpr unique_array_t( unique_array_t &&other ) noexcept
		  : m_data( daw::exchange( other.m_data, nullptr ) )
		  , m_size( daw::exchange( other.m_size, 0 ) ) {}

		constexpr unique_array_t &operator=( unique_array_t &&rhs ) noexcept {
			if( this == &rhs ) {
				return *this;
			}
			clear( );
			if( !rhs.m_data ) {
				return *this;
			}
			m_data = daw::exchange( rhs.m_data, nullptr );
			m_size = daw::exchange( rhs.m_size, 0 );
			return *this;
		}

		unique_array_t( size_type sz ) noexcept
		  : m_data( create_values( sz ) )
		  , m_size( sz ) {}

		template<typename... Args>
		unique_array_t( size_type sz, Args &&... args ) noexcept(
		  std::is_nothrow_constructible_v<value_type, Args...> )
		  : m_data( create_values( sz ) )
		  , m_size( sz ) {

			std::fill_n( m_data, m_size, { args... } );
		}

		~unique_array_t( ) noexcept( noexcept( delete[] m_data ) ) {
			clear( );
		}

		unique_array_t( unique_array_t const & ) = delete;
		unique_array_t &operator=( unique_array_t const & ) = delete;

		constexpr bool empty( ) const noexcept {
			return m_data == nullptr;
		}

		constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr size_type capacity( ) const noexcept {
			return m_size;
		}

		reference operator[]( size_type pos ) noexcept {
			return m_data[pos];
		}

		const_reference operator[]( size_type pos ) const noexcept {
			return m_data[pos];
		}

		reference at( size_type pos ) {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos >= m_size, "Attempt to access value past end of range" );
			return m_data[pos];
		}

		const_reference at( size_type pos ) const {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos >= m_size, "Attempt to access value past end of range" );
			return m_data[pos];
		}

		constexpr iterator begin( ) noexcept {
			return m_data;
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_data;
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_data;
		}

		constexpr iterator end( ) noexcept {
			return std::next( m_data, m_size );
		}

		constexpr const_iterator end( ) const noexcept {
			return std::next( m_data, m_size );
		}

		constexpr const_iterator cend( ) const noexcept {
			return std::next( m_data, m_size );
		}

		reference front( ) noexcept {
			return *m_data;
		}

		const_reference front( ) const noexcept {
			return *m_data;
		}

		reference back( ) noexcept {
			return *std::next( m_data, m_size - 1 );
		}

		const_reference back( ) const noexcept {
			return *std::next( m_data, m_size - 1 );
		}
	};
} // namespace daw

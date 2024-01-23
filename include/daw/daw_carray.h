// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_swap.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <stdexcept>

namespace daw {
	namespace details {
		template<typename T, typename Arg>
		constexpr void assign_to( T *ptr, Arg &&arg ) noexcept(
		  noexcept( T( std::declval<Arg &&>( ) ) ) ) {
			*ptr = DAW_FWD( arg );
		}

		template<typename T, typename Arg, typename... Args>
		constexpr void assign_to( T *ptr, Arg &&arg, Args &&...args ) noexcept(
		  noexcept( T( std::declval<Arg &&>( ) ) ) ) {
			*ptr = DAW_FWD( arg );
			assign_to( ++ptr, DAW_FWD( args )... );
		}
	} // namespace details

	template<typename T, size_t N>
	struct carray {
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using pointer = T *;
		using const_pointer = T const *;
		using size_type = std::size_t;

	private:
		alignas( value_type ) std::array<uint8_t, sizeof( T ) * N> m_data;

		constexpr pointer ptr( ) noexcept {
			return reinterpret_cast<pointer>( m_data.data( ) );
		}

		constexpr const_pointer ptr( ) const noexcept {
			return reinterpret_cast<const_pointer>( m_data.data( ) );
		}

		template<typename Iterator>
		static constexpr std::reverse_iterator<Iterator>
		make_reverse_iterator( Iterator i ) {
			return std::reverse_iterator<Iterator>( i );
		}

	public:
		constexpr carray( ) noexcept
		  : m_data{ } {}

		constexpr carray( T ( &values )[N] ) noexcept(
		  noexcept( T( std::declval<T const &>( ) ) ) )
		  : m_data{ } {
			daw::algorithm::copy_n( values, ptr( ), N );
		}

		constexpr carray &operator=( T ( &values )[N] ) noexcept(
		  noexcept( T( std::declval<T const &>( ) ) ) ) {
			daw::algorithm::copy_n( values, ptr( ), N );
			return *this;
		}

		template<typename... Args>
		constexpr carray( Args &&...args ) noexcept( noexcept( details::assign_to(
		  std::declval<carray>( ).ptr( ), std::declval<Args &&>( )... ) ) )
		  : m_data{ } {

			details::assign_to( ptr( ), DAW_FWD( args )... );
		}

		~carray( ) noexcept = default;

		constexpr carray( carray const &other ) noexcept(
		  noexcept( T( std::declval<T const &>( ) ) ) ) {
			daw::algorithm::copy_n( other.ptr( ), ptr( ), N );
		}

		constexpr carray &operator=( carray const &other ) noexcept(
		  noexcept( T( std::declval<T const &>( ) ) ) ) {
			daw::algorithm::copy_n( other.ptr( ), ptr( ), N );
			return *this;
		}

		constexpr carray( carray && ) noexcept(
		  noexcept( T( std::declval<T &&>( ) ) ) ) = default;

		constexpr carray &operator=( carray && ) noexcept(
		  noexcept( T( std::declval<T &&>( ) ) ) ) = default;

		[[nodiscard]] constexpr reference operator[]( size_type pos ) noexcept {
			return ptr( )[pos];
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type pos ) const noexcept {
			return ptr( )[pos];
		}

		[[nodiscard]] constexpr reference at( size_type pos ) {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos < N, "Attemp to access past end of array" );
			return ptr( )[pos];
		}

		[[nodiscard]] constexpr const_reference at( size_type pos ) const {
			daw::exception::daw_throw_on_false<std::out_of_range>(
			  pos < N, "Attemp to access past end of array" );
			return ptr( )[pos];
		}

		[[nodiscard]] constexpr pointer begin( ) noexcept {
			return ptr( );
		}

		[[nodiscard]] constexpr const_pointer begin( ) const noexcept {
			return ptr( );
		}

		[[nodiscard]] constexpr const_pointer cbegin( ) const noexcept {
			return ptr( );
		}

		[[nodiscard]] constexpr pointer end( ) noexcept {
			return &ptr( )[N];
		}

		[[nodiscard]] constexpr const_pointer end( ) const noexcept {
			return &ptr( )[N];
		}

		[[nodiscard]] constexpr const_pointer cend( ) const noexcept {
			return &ptr( )[N];
		}

		[[nodiscard]] constexpr auto rbegin( ) noexcept {
			static_assert( N > 0, "Cannot call rbegin on empty array" );
			return make_reverse_iterator( &ptr( )[N - 1] );
		}

		[[nodiscard]] constexpr auto rbegin( ) const noexcept {
			static_assert( N > 0, "Cannot call rbegin on empty array" );
			return make_reverse_iterator( &ptr( )[N - 1] );
		}

		[[nodiscard]] constexpr auto crbegin( ) const noexcept {
			static_assert( N > 0, "Cannot call crbegin on empty array" );
			return make_reverse_iterator( &ptr( )[N - 1] );
		}

		[[nodiscard]] constexpr auto rend( ) noexcept {
			static_assert( N > 0, "Cannot call rend on empty array" );
			return make_reverse_iterator( ptr( ) - 1 );
		}

		[[nodiscard]] constexpr auto rend( ) const noexcept {
			static_assert( N > 0, "Cannot call rend on empty array" );
			return make_reverse_iterator( ptr( ) - 1 );
		}

		[[nodiscard]] constexpr auto crend( ) const noexcept {
			static_assert( N > 0, "Cannot call crend on empty array" );
			return make_reverse_iterator( ptr( ) - 1 );
		}

		[[nodiscard]] constexpr reference front( ) noexcept {
			static_assert( N > 0, "Cannot call front on empty array" );
			return ptr( )[0];
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			static_assert( N > 0, "Cannot call front on empty array" );
			return ptr( )[0];
		}

		[[nodiscard]] constexpr reference back( ) noexcept {
			static_assert( N > 0, "Cannot call back on empty array" );
			return ptr( )[N - 1];
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			static_assert( N > 0, "Cannot call back on empty array" );
			return ptr( )[N - 1];
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return ptr( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return N;
		}

		[[nodiscard]] constexpr size_type max_size( ) const noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return 0 == N;
		}

		constexpr void fill( const_reference value ) noexcept(
		  noexcept( T( std::declval<T const &>( ) ) ) ) {
			std::fill( begin( ), end( ), value );
		}
	}; // carray

	template<typename T, size_t N>
	constexpr void
	swap( carray<T, N> &lhs,
	      carray<T, N> &rhs ) noexcept( std::is_nothrow_move_assignable_v<T> ) {
		daw::swap_ranges( lhs.begin( ), lhs.end( ), rhs.begin( ) );
	}
} // namespace daw

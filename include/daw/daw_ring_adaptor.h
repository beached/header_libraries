// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_construct_a.h"
#include "daw_move.h"
#include "iterator/daw_indexed_iterator.h"

#include <array>
#include <cassert>
#include <iterator>

namespace daw {
	template<typename Container>
	struct ring_adaptor {
		using container_type = typename container_traits<Container>::container_type;
		using value_type = typename container_traits<Container>::value_type;
		using reference = typename container_traits<Container>::reference;
		using const_reference =
		  typename container_traits<Container>::const_reference;
		using iterator = indexed_iterator<ring_adaptor>;
		using const_iterator = indexed_iterator<ring_adaptor const>;
		using size_type = typename container_traits<Container>::size_type;
		using difference_type =
		  typename container_traits<Container>::difference_type;
		static constexpr std::size_t extent = container_traits<Container>::extent;

	private:
		container_type m_queue{ };
		size_t m_head = 0;
		size_t m_tail = 0;

		inline constexpr size_type get_idx( size_type i ) const {
			return i % capacity( );
		}

	public:
		constexpr ring_adaptor( ) = default;

		template<typename Arg, typename... Args,
		         std::enable_if_t<
		           not std::is_same_v<ring_adaptor, daw::remove_cvref_t<Arg>>,
		           std::nullptr_t> = nullptr>
		inline constexpr explicit ring_adaptor( Arg &&arg, Args &&...args )
		  : m_queue( DAW_FWD( arg ), DAW_FWD( args )... ) {}

		inline constexpr container_type &underlying_container( ) {
			return m_queue;
		}

		inline constexpr container_type const &underlying_container( ) const {
			return m_queue;
		}

		inline constexpr reference operator[]( std::size_t idx ) {
			assert( idx < size( ) );
			return m_queue[get_idx( m_head + idx )];
		}

		inline constexpr const_reference operator[]( std::size_t idx ) const {
			assert( idx < size( ) );
			return m_queue[get_idx( m_head + idx )];
		}

		inline constexpr size_type capacity( ) const {
			if constexpr( extent == DynamicExtent ) {
				return std::size( m_queue );
			} else {
				return extent;
			}
		}

		inline constexpr size_type size( ) const {
			return m_tail - m_head;
		}

		inline constexpr bool is_empty( ) const {
			return m_head == m_tail;
		}

		inline constexpr bool is_full( ) const {
			return size( ) == capacity( );
		}

		inline constexpr void clear( ) {
			m_head = 0;
			m_tail = 0;
		}

		template<typename... Args>
		reference emplace_back( Args &&...args ) {
			assert( not is_full( ) );
			auto const idx = get_idx( m_head );
			++m_head;
			m_queue[idx] = construct_a<value_type>( DAW_FWD( args )... );
			return m_queue[idx];
		}

		reference push_back( value_type const &value ) {
			assert( not is_full( ) );
			auto const idx = get_idx( m_tail );
			m_queue[idx] = value;
			++m_tail;
			return m_queue[idx];
		}

		reference push_back( value_type &&value ) {
			assert( not is_full( ) );
			auto const idx = get_idx( m_tail );
			m_queue[idx] = std::move( value );
			++m_tail;
			return m_queue[idx];
		}

		value_type pop_front( ) {
			assert( not is_empty( ) );
			auto const idx = get_idx( m_head );
			++m_head;
			return daw::move( m_queue[idx] );
		}

		inline constexpr const_iterator begin( ) const {
			return const_iterator( this, 0 );
		}

		inline constexpr iterator begin( ) {
			return iterator( this, 0 );
		}

		inline constexpr const_iterator end( ) const {
			return const_iterator( this, size( ) );
		}

		inline constexpr iterator end( ) {
			return iterator( this, size( ) );
		}
	};

	template<typename T>
	ring_adaptor( T const & ) -> ring_adaptor<T>;
} // namespace daw

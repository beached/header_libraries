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
#include "traits/daw_traits_conditional.h"
#include "wrap_iter.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

namespace daw {
	namespace iter_view_details {
		template<typename Iterator>
		struct find_raw_result {
			Iterator iterator;
			std::size_t count_from_start;
		};
	} // namespace iter_view_details
	template<typename ForwardIterator, typename Last = ForwardIterator,
	         bool ExplicitConv = false>
	struct iter_view {
		using iterator = ForwardIterator;
		using const_iterator = iterator;
		using value_type = typename std::iterator_traits<iterator>::value_type;
		using reference = typename std::iterator_traits<iterator>::reference;
		using const_reference =
		  conditional_t<std::is_rvalue_reference_v<reference>,
		                std::add_rvalue_reference_t<
		                  std::add_const_t<std::remove_reference_t<reference>>>,
		                std::add_lvalue_reference_t<
		                  std::add_const_t<std::remove_reference_t<reference>>>>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using last_iterator = Last;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	private:
		iterator m_first = { };
		last_iterator m_last = { };
		size_type m_size = distance( m_first, m_last );

		static constexpr size_type distance( iterator f, last_iterator l ) {
#if not defined( __cpp_lib_ranges )
			if constexpr( std::is_same_v<iterator, last_iterator> ) {
				return static_cast<size_type>( std::distance( f, l ) );
			} else if constexpr( requires {
				                     { l - f } -> std::convertible_to<difference_type>;
			                     } ) {
				return static_cast<size_type>( l - f );
			} else {
				size_type count = 0;
				while( f != l ) {
					++count;
					std::advance( f, 1 );
				}
				return count;
			}
#if __cpp_lib_ranges < 201911L
#else
			return static_cast<size_type>( std::ranges::distance( f, l ) );
#endif
#else
			return static_cast<size_type>( std::ranges::distance( f, l ) );
#endif
		}

	public:
		explicit iter_view( ) = default;

		constexpr iter_view( iterator first, last_iterator last ) noexcept
		  : m_first( first )
		  , m_last( last ) {}

		constexpr iter_view( iterator first, size_type sz ) noexcept
		  : m_first( first )
		  , m_last( std::next( first, sz ) )
		  , m_size( sz ) {}

		template<typename Container>
		requires(
		  ( Containers<Container> or ContiguousContainer<Container> ) and
		  not_cvref_of<
		    iter_view,
		    Container> ) explicit( ExplicitConv ) constexpr iter_view( Container &&
		                                                                 c ) noexcept
		  : m_first( std::begin( c ) )
		  , m_last( std::end( c ) ) {}

		constexpr iter_view
		subspan( size_type offset,
		         size_type count = std::numeric_limits<size_type>::max( ) ) const {
			auto const sz = std::min( count, size( ) - std::min( size( ), offset ) );
			return iter_view( std::next( begin( ), offset ),
			                  std::next( begin( ), sz ) );
		}

		constexpr size_type size( ) const {
			return m_size;
		}

		/// This is potentially unsafe.  Decrements the start of range
		constexpr void add_prefix( ) noexcept {
			std::advance( m_first, -1 );
			++m_size;
		}

		/// This is potentially unsafe.  Decrements the start of range
		constexpr void add_prefix( size_type n ) noexcept {
			std::advance( m_first, -static_cast<difference_type>( n ) );
			m_size += n;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_first;
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
			return m_last;
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return m_last;
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

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_size == 0;
		}

		[[nodiscard]] constexpr reference front( ) {
			assert( not empty( ) );
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference front( ) const {
			assert( not empty( ) );
			return *m_first;
		}

		constexpr reference operator[]( size_type idx ) {
			assert( idx < size( ) );
			return *std::next( m_first, static_cast<difference_type>( idx ) );
		}

		constexpr const_reference operator[]( size_type idx ) const {
			assert( idx < size( ) );
			return *std::next( m_first, static_cast<difference_type>( idx ) );
		}

		constexpr void remove_prefix( size_type count ) {
			count = ( std::min )( count, size( ) );
			std::advance( m_first, count );
			m_size -= count;
		}

		constexpr void remove_prefix( size_type count, dont_clip_to_bounds_t ) {
			assert( count < size( ) );
			std::advance( m_first, static_cast<difference_type>( count ) );
			m_size -= count;
		}

		constexpr void remove_prefix( ) {
			assert( not empty( ) );
			remove_prefix( 1 );
		}

		[[nodiscard]] constexpr value_type pop_front( ) {
			assert( not empty( ) );
			auto ret_it = m_first;
			std::advance( m_first, 1 );
			--m_size;
			return *ret_it;
		}

	private:
		[[nodiscard]] constexpr iter_view_details::find_raw_result<iterator>
		find_raw( const_reference needle ) {
			auto result = iter_view_details::find_raw_result<iterator>{ m_first, 0 };
			while( result.iterator != m_last and *result.iterator != needle ) {
				std::advance( result.iterator, 1 );
				++result.count_from_start;
			}
			return result;
		}

		[[nodiscard]] constexpr iter_view_details::find_raw_result<const_iterator>
		find_raw( const_reference needle ) const {
			auto result =
			  iter_view_details::find_raw_result<const_iterator>{ m_first, 0 };
			while( result.iterator != m_last and *result.iterator != needle ) {
				std::advance( result.iterator, 1 );
				++result.count_from_start;
			}
			return result;
		}

		[[nodiscard]] constexpr iter_view_details::find_raw_result<iterator>
		find_first_not_of_raw( const_reference needle ) {
			auto result = iter_view_details::find_raw_result<iterator>{ m_first, 0 };
			while( result.iterator != m_last and *result.iterator == needle ) {
				std::advance( result.iterator, 1 );
				++result.count_from_start;
			}
			return result;
		}

		[[nodiscard]] constexpr iter_view_details::find_raw_result<const_iterator>
		find_first_not_of_raw( const_reference needle ) const {
			auto result =
			  iter_view_details::find_raw_result<const_iterator>{ m_first, 0 };
			while( result.iterator != m_last and *result.iterator == needle ) {
				std::advance( result.iterator, 1 );
				++result.count_from_start;
			}
			return result;
		}

	public:
		[[nodiscard]] constexpr iterator find( const_reference needle ) {
			return iterator{ find_raw( needle ).iterator };
		}

		[[nodiscard]] constexpr const_iterator
		find( const_reference needle ) const {
			return const_iterator{ find_raw( needle ).iterator };
		}

		[[nodiscard]] constexpr iterator
		find_first_not_of( const_reference needle ) {
			return iterator{ find_first_not_of_raw( needle ).iterator };
		}

		[[nodiscard]] constexpr const_iterator
		find_first_not_of( const_reference needle ) const {
			return const_iterator{ find_first_not_of_raw( needle ).iterator };
		}

		constexpr iter_view &remove_prefix_until( const_reference needle,
		                                          discard_t ) {
			auto const raw_result = find_raw( needle );
			m_first = raw_result.iterator;
			m_size -= raw_result.count_from_start;
			if( not empty( ) ) {
				std::advance( m_first, 1 );
				--m_size;
			}
			return *this;
		}

		constexpr iter_view &remove_prefix_until( const_reference needle ) {
			auto const raw_result = find_raw( needle );
			m_first = raw_result.iterator;
			m_size -= raw_result.count_from_start;
			return *this;
		}

		constexpr iter_view &remove_prefix_while( const_reference needle,
		                                          discard_t ) {
			auto const raw_result = find_first_not_of_raw( needle );
			m_first = raw_result.iterator;
			m_size -= raw_result.count_from_start;
			if( not empty( ) ) {
				std::advance( m_first, 1 );
				--m_size;
			}
			return *this;
		}

		constexpr iter_view &remove_prefix_while( const_reference needle ) {
			auto const raw_result = find_first_not_of_raw( needle );
			m_first = raw_result.iterator;
			m_size -= raw_result.count_from_start;
			return *this;
		}

		[[nodiscard]] constexpr iter_view
		pop_front_until( const_reference needle ) {
			auto const f = m_first;
			remove_prefix_until( needle );
			return iter_view( f, m_first );
		}

		[[nodiscard]] constexpr iter_view pop_front_until( const_reference needle,
		                                                   discard_t ) {
			auto const f = m_first;
			remove_prefix_until( needle );
			auto result = iter_view( f, m_first );
			remove_prefix( );
			return result;
		}

		[[nodiscard]] constexpr iter_view
		pop_front_while( const_reference needle ) {
			auto const f = m_first;
			remove_prefix_while( needle );
			return iter_view( f, m_first );
		}

		[[nodiscard]] constexpr iter_view pop_front_while( const_reference needle,
		                                                   discard_t ) {
			auto const f = m_first;
			remove_prefix_while( needle );
			auto result = iter_view( f, m_first );
			remove_prefix( );
			return result;
		}

	private:
		[[nodiscard]] static constexpr bool equal( iterator lf, last_iterator ll,
		                                           iterator rf, last_iterator rl ) {
			while( lf != ll and rf != rl ) {
				if( *lf != *rf ) {
					return false;
				}
				std::advance( lf, 1 );
				std::advance( rf, 1 );
			}
			return true;
		}

		[[nodiscard]] friend constexpr bool operator==( iter_view const &x,
		                                                iter_view const &y ) {
			return x.size( ) == y.size( ) and
			       std::equal( x.begin( ), x.end( ), y.begin( ), y.end( ) );
		}

		[[nodiscard]] friend constexpr bool operator!=( iter_view const &x,
		                                                iter_view const &y ) {
			return not( x == y );
		}

		[[nodiscard]] friend constexpr bool operator<( iter_view const &x,
		                                               iter_view const &y ) {
			return std::lexicographical_compare( x.begin( ), x.end( ), y.begin( ),
			                                     y.end( ) );
		}

		[[nodiscard]] friend constexpr bool operator>( iter_view const &x,
		                                               iter_view const &y ) {
			return y < x;
		}

		[[nodiscard]] friend constexpr bool operator>=( iter_view const &x,
		                                                iter_view const &y ) {
			return not( x < y );
		}

		[[nodiscard]] friend constexpr bool operator<=( iter_view const &x,
		                                                iter_view const &y ) {
			return not( y < x );
		}
	};

	template<typename ForwardIterator, typename Last>
	iter_view( ForwardIterator, Last ) -> iter_view<ForwardIterator, Last>;

	template<typename Container>
	iter_view( Container ) -> iter_view<typename Container::iterator>;

	template<typename T, std::size_t N>
	iter_view( T ( & )[N] ) -> iter_view<T *>;

	template<typename T, std::size_t N>
	iter_view( T ( && )[N] ) -> iter_view<T *>;

} // namespace daw

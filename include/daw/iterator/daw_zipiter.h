// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"
#include "../cpp_17.h"
#include "../daw_algorithm.h"
#include "../daw_move.h"
#include "../daw_traits.h"
#include "../daw_tuple_helper.h"
#include "../daw_utility.h"

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace daw {
	namespace zipiter_details {
		template<typename Iterator>
		constexpr decltype( auto ) get_pointer( Iterator &&iterator ) {
			if constexpr( std::is_pointer_v<daw::remove_cvref_t<Iterator>> ) {
				return iterator;
			} else {
				return iterator.operator->( );
			}
		}
	} // namespace zipiter_details
	template<typename... Iterators>
	struct zip_iterator {
		static_assert( sizeof...( Iterators ) > 0,
		               "Empty zip iterator is unsupported" );
		using types_t = std::tuple<Iterators...>;
		using value_type = types_t;
		using reference = std::tuple<decltype( *std::declval<Iterators &>( ) )...>;
		using const_reference =
		  std::tuple<decltype( *std::declval<Iterators const &>( ) )...>;
		using pointer = std::tuple<decltype( zipiter_details::get_pointer(
		  std::declval<Iterators &>( ) ) )...>;
		using const_pointer = std::tuple<decltype( zipiter_details::get_pointer(
		  std::declval<Iterators const &>( ) ) )...>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;

	private:
		types_t m_values;

		template<typename... Ts, std::size_t... Is>
		static constexpr void increment( std::tuple<Ts...> &tpl,
		                                 std::index_sequence<Is...> ) {
			::Unused( ( ( ++std::get<Is>( tpl ), 0 ) + ... ) );
		}

		template<typename... Ts>
		static constexpr void increment( std::tuple<Ts...> &tpl ) {
			increment( tpl, std::index_sequence_for<Ts...>{ } );
		}

		template<typename... Ts, std::size_t... Is>
		static constexpr void decrement( std::tuple<Ts...> &tpl,
		                                 std::index_sequence<Is...> ) {
			::Unused( ( ( --std::get<Is>( tpl ), 0 ) + ... ) );
		}

		template<typename... Ts>
		static constexpr void decrement( std::tuple<Ts...> &tpl ) {
			decrement( tpl, std::index_sequence_for<Ts...>{ } );
		}

		template<typename... Ts, std::size_t... Is>
		static constexpr void advance( std::tuple<Ts...> &tpl,
		                               std::index_sequence<Is...>, intmax_t n ) {

			::Unused( ( ( daw::advance( std::get<Is>( tpl ), n ), 0 ) + ... ) );
		}

		template<typename... Ts>
		static constexpr void advance( std::tuple<Ts...> &tpl, intmax_t n ) {
			advance( tpl, std::index_sequence_for<Ts...>{ }, n );
		}

	public:
		constexpr zip_iterator( Iterators const &...its )
		  : m_values( DAW_MOVE( its )... ) {}

		constexpr types_t &as_tuple( ) noexcept {
			return m_values;
		}

		constexpr types_t const &as_tuple( ) const noexcept {
			return m_values;
		}

		constexpr zip_iterator &operator++( ) {
			increment( m_values );
			return *this;
		}

		constexpr zip_iterator operator++( int ) {
			auto tmp = *this;
			increment( m_values );
			return tmp;
		}

		constexpr zip_iterator &operator--( ) {
			decrement( m_values );
			return *this;
		}

		constexpr zip_iterator operator--( int ) {
			auto tmp = *this;
			decrement( m_values );
			return tmp;
		}

		constexpr void advance( intmax_t n ) {
			advance( m_values, n );
		}

		template<size_t pos>
		constexpr decltype( auto ) get( ) noexcept {
			return std::get<pos>( m_values );
		}

		template<size_t pos>
		constexpr decltype( auto ) get( ) const noexcept {
			return std::get<pos>( m_values );
		}

		constexpr static size_t size( ) noexcept {
			return sizeof...( Iterators );
		}

	private:
		template<size_t... Is>
		constexpr reference get_items( std::index_sequence<Is...> ) noexcept {
			return { *std::get<Is>( m_values )... };
		}

		template<size_t... Is>
		constexpr const_reference
		get_items( std::index_sequence<Is...> ) const noexcept {
			return { *std::get<Is>( m_values )... };
		}

	public:
		constexpr reference operator*( ) noexcept {
			return get_items(
			  std::make_index_sequence<daw::tuple_size_v<types_t>>( ) );
		}

		constexpr const_reference operator*( ) const noexcept {
			return get_items(
			  std::make_index_sequence<daw::tuple_size_v<types_t>>( ) );
		}

		constexpr pointer operator->( ) noexcept {
			return { operator*( ) };
		}

		constexpr const_pointer operator->( ) const noexcept {
			return { operator*( ) };
		}

		constexpr bool operator==( zip_iterator const &rhs ) const noexcept {
			return m_values == rhs.m_values;
		}

		constexpr bool operator!=( zip_iterator const &rhs ) const noexcept {
			return m_values != rhs.m_values;
		}
	}; // struct zip_iterator

	template<typename... Iterators>
	zip_iterator( Iterators... ) -> zip_iterator<Iterators...>;

	template<size_t N, typename... T>
	constexpr decltype( auto ) get( zip_iterator<T...> &zi ) noexcept {
		return zi.template get<N>( );
	}

	template<size_t N, typename... T>
	constexpr decltype( auto ) get( zip_iterator<T...> const &zi ) noexcept {
		return zi.template get<N>( );
	}
} // namespace daw

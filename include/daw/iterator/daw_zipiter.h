// The MIT License (MIT)
//
// Copyright (c) 2013-2020 Darrell Wright
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

#include <tuple>

#include "../cpp_17.h"
#include "../daw_algorithm.h"
#include "../daw_move.h"
#include "../daw_tuple_helper.h"
#include "../daw_utility.h"

namespace daw {
	template<typename... Iterators>
	struct zip_iterator {
		static_assert( sizeof...( Iterators ) > 0,
		               "Empty zip iterator is unsupported" );
		using types_t = std::tuple<Iterators...>;

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
		template<
		  typename... Its,
		  std::enable_if_t<
		    daw::all_true_v<
		      ( sizeof...( Its ) > 0 ),
		      !std::is_same_v<zip_iterator, daw::remove_cvref_t<
		                                      daw::traits::first_type<Its...>>>>,
		    std::nullptr_t> = nullptr>
		constexpr zip_iterator( Its &&... its )
		  : m_values( std::forward<Its>( its )... ) {}

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
			return daw::tuple_size_v<Iterators...>;
		}

	private:
		template<size_t... Is>
		constexpr auto get_items( std::index_sequence<Is...> ) noexcept {
			return std::forward_as_tuple( *std::get<Is>( m_values )... );
		}

		template<size_t... Is>
		constexpr auto get_items( std::index_sequence<Is...> ) const noexcept {
			return std::forward_as_tuple( *std::get<Is>( m_values )... );
		}

	public:
		constexpr decltype( auto ) operator*( ) noexcept {
			return get_items(
			  std::make_index_sequence<daw::tuple_size_v<types_t>>( ) );
		}

		constexpr decltype( auto ) operator*( ) const noexcept {
			return get_items(
			  std::make_index_sequence<daw::tuple_size_v<types_t>>( ) );
		}
	}; // struct zip_iterator

	template<typename... Iterators>
	zip_iterator( Iterators &&... )
	  -> zip_iterator<std::remove_reference_t<Iterators>...>;

	template<typename... T>
	constexpr bool operator==( zip_iterator<T...> const &lhs,
	                           zip_iterator<T...> const &rhs ) noexcept {
		return lhs.as_tuple( ) == rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator!=( zip_iterator<T...> const &lhs,
	                           zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) != rhs.as_tuple( );
	}

	template<size_t N, typename... T>
	constexpr decltype( auto ) get( zip_iterator<T...> &zi ) noexcept {
		return zi.template get<N>( );
	}

	template<size_t N, typename... T>
	constexpr decltype( auto ) get( zip_iterator<T...> const &zi ) noexcept {
		return zi.template get<N>( );
	}
} // namespace daw

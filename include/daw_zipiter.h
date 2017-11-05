// The MIT License ( MIT )
//
// Copyright ( c ) 2013-2017 Darrell Wright
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

#include <tuple>

#include "cpp_17.h"
#include "daw_algorithm.h"

namespace daw {
	template<typename... Iterators>
	struct zip_iterator {
		using types_t = std::tuple<Iterators...>;

	private:
		types_t m_values;

		template<typename... Ts, std::size_t... Is>
		static constexpr void increment( std::tuple<Ts...> &tpl, std::index_sequence<Is...> ) {
			using expander = int[];
			expander{0, ( static_cast<void>( ++std::get<Is>( tpl ) ), 0 )...};
		}

		template<typename... Ts>
		static constexpr void increment( std::tuple<Ts...> &tpl ) {
			increment( tpl, std::index_sequence_for<Ts...>{} );
		}

		template<typename... Ts, std::size_t... Is>
		static constexpr void decrement( std::tuple<Ts...> &tpl, std::index_sequence<Is...> ) {
			using expander = int[];
			expander{0, ( static_cast<void>( --std::get<Is>( tpl ) ), 0 )...};
		}

		template<typename... Ts>
		static constexpr void decrement( std::tuple<Ts...> &tpl ) {
			decrement( tpl, std::index_sequence_for<Ts...>{} );
		}

		template<typename... Ts, std::size_t... Is>
		static constexpr void advance( std::tuple<Ts...> &tpl, std::index_sequence<Is...>, intmax_t n ) {
			using expander = int[];
			expander{0, ( static_cast<void>( ::daw::algorithm::impl::advance( std::get<Is>( tpl ), n, std::random_access_iterator_tag{} ) ), 0 )...};
		}

		template<typename... Ts>
		static constexpr void advance( std::tuple<Ts...> &tpl, intmax_t n ) {
			advance( tpl, std::index_sequence_for<Ts...>{}, n );
		}

	public:
		zip_iterator( ) = delete;
		~zip_iterator( ) = default;
		constexpr zip_iterator( zip_iterator const & ) = default;
		constexpr zip_iterator( zip_iterator && ) noexcept = default;
		constexpr zip_iterator &operator=( zip_iterator const & ) = default;
		constexpr zip_iterator &operator=( zip_iterator && ) noexcept = default;

		constexpr zip_iterator( Iterators... args ) : m_values{std::move( args )...} {}

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
			++( *this );
			return tmp;
		}

		constexpr zip_iterator &operator--( ) {
			decrement( m_values );
			return *this;
		}

		constexpr zip_iterator operator--( int ) {
			auto tmp = *this;
			--( *this );
			return tmp;
		}

		constexpr void advance( intmax_t n ) {
			advance( m_values, n );
		}

		template<size_t pos>
		constexpr auto &get( ) noexcept {
			return std::get<pos>( m_values );
		}

		constexpr static size_t size( ) noexcept {
			return tuple_size_v<Iterators...>;
		}
	}; // struct zip_iterator

	template<typename... T>
	constexpr bool operator==( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) noexcept {
		return lhs.as_tuple( ) == rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator!=( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) != rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator<=( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) <= rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator>=( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) >= rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator<( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) < rhs.as_tuple( );
	}

	template<typename... T>
	constexpr bool operator>( zip_iterator<T...> const &lhs, zip_iterator<T...> const &rhs ) {
		return lhs.as_tuple( ) > rhs.as_tuple( );
	}

	template<typename... T>
	constexpr zip_iterator<T...> make_zip_iterator( T... args ) {
		return zip_iterator<T...>( args... );
	}
} // namespace daw

template<size_t i, typename... T>
constexpr auto &get( daw::zip_iterator<T...> &zi ) noexcept {
	return zi.template get<i>( );
}

template<size_t i, typename... T>
constexpr auto const &get( daw::zip_iterator<T...> const &zi ) noexcept {
	return zi.template get<i>( );
}


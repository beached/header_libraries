// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include <string>

#include "daw_algorithm.h"
#include "daw_move.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "iterator/daw_reverse_iterator.h"

namespace daw {
	template<typename CharT>
	class string_split_iterator {
		static constexpr size_t const npos = std::numeric_limits<size_t>::max( );

		daw::basic_string_view<CharT> m_str{};
		daw::basic_string_view<CharT> m_delemiter{};
		size_t m_pos = npos;

		constexpr size_t find_next( ) const noexcept {
			if( m_str.empty( ) ) {
				return npos;
			}
			if constexpr( std::is_same_v<CharT, Delemiter> ) {
				return m_str.find( m_delemiter, m_pos );
			} else {
				return m_str.find( daw::make_string_view( m_delemiter ), m_pos );
			}
		}

		constexpr void move_next( ) {
			auto next_pos = find_next( );
			if( next_pos == npos ) {
				m_pos = npos;
				return;
			}
			if constexpr( std::is_same_v<CharT, Delemiter> ) {
				m_pos = next_pos + 1;
			} else {
				m_pos = next_pos + std::size( m_delemiter );
			}
		}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::basic_string_view<CharT>;
		using reference = daw::basic_string_view<CharT>;
		using pointer = daw::basic_string_view<CharT>;
		using difference_type = ptrdiff_t;

		constexpr string_split_iterator( ) noexcept = default;

		constexpr string_split_iterator( daw::basic_string_view<CharT> str,
		                                 Delemiter delemiter ) noexcept
		  : m_str( str )
		  , m_delemiter( delemiter )
		  , m_pos( 0 ) {}

		string_split_iterator( std::basic_string<CharT> const &str,
		                       Delemiter delemiter ) noexcept
		  : m_str( make_string_view( str ) )
		  , m_delemiter( delemiter )
		  , m_pos( 0 ) {}

		constexpr string_split_iterator( daw::basic_string_view<CharT> str,
		                                 Delemiter &&delemiter ) noexcept
		  : m_str( str )
		  , m_delemiter( daw::move( delemiter ) )
		  , m_pos( 0 ) {}

		string_split_iterator( std::basic_string<CharT> const &str,
		                       Delemiter &&delemiter ) noexcept
		  : m_str( make_string_view( str ) )
		  , m_delemiter( daw::move( delemiter ) )
		  , m_pos( 0 ) {}

		constexpr string_split_iterator &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr string_split_iterator operator++( int ) const noexcept {
			string_split_iterator tmp{*this};
			tmp.move_next( );
			return tmp;
		}

		constexpr daw::basic_string_view<CharT> operator*( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		constexpr daw::basic_string_view<CharT> operator->( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		constexpr bool equal_to( string_split_iterator const &rhs ) const noexcept {
			if( m_pos == npos and rhs.m_pos == npos ) {
				return true;
			}
			return std::tie( m_str, m_pos, m_delemiter ) ==
			       std::tie( rhs.m_str, rhs.m_pos, rhs.m_delemiter );
		}
	};

	template<typename CharT, typename Delemiter>
	string_split_iterator( daw::basic_string_view<CharT>, Delemiter && )
	  ->string_split_iterator<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, typename Delemiter>
	string_split_iterator( std::basic_string<CharT>, Delemiter && )
	  ->string_split_iterator<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, typename DelemiterL, typename DelemiterR>
	constexpr bool
	operator==( string_split_iterator<CharT, DelemiterL> const &lhs,
	            string_split_iterator<CharT, DelemiterR> const &rhs ) noexcept {
		if constexpr( !std::is_same_v<DelemiterL, DelemiterR> ) {
			return false;
		}
		return lhs.equal_to( rhs );
	}

	template<typename CharT, typename DelemiterL, typename DelemiterR>
	constexpr bool
	operator!=( string_split_iterator<CharT, DelemiterL> const &lhs,
	            string_split_iterator<CharT, DelemiterR> const &rhs ) noexcept {
		if constexpr( !std::is_same_v<DelemiterL, DelemiterR> ) {
			return true;
		}
		return !lhs.equal_to( rhs );
	}

	template<typename CharT, typename Delemiter>
	struct string_split_range {
		using iterator = string_split_iterator<CharT, Delemiter>;

	private:
		iterator m_first{};

	public:
		constexpr string_split_range( ) noexcept = default;

		constexpr string_split_range( daw::basic_string_view<CharT> str,
		                              Delemiter delemiter ) noexcept
		  : m_first( str, delemiter ) {}

		template<size_t N>
		constexpr string_split_range( daw::basic_string_view<CharT> str,
		                              CharT const ( &delemiter )[N] ) noexcept
		  : m_first( str, daw::basic_string_view<CharT, std::char_traits<CharT>, N>( delemiter ) ) {}

		string_split_range( std::basic_string<CharT> const &str,
		                    Delemiter delemiter ) noexcept
		  : m_first( str, delemiter ) {}

		template<size_t N>
		string_split_range( std::basic_string<CharT> str,
		                    CharT const ( &delemiter )[N] ) noexcept
		  : m_first( str, delemiter ) {}

		constexpr iterator begin( ) const {
			return m_first;
		}

		constexpr iterator cbegin( ) const {
			return m_first;
		}

		constexpr iterator end( ) const {
			return iterator{};
		}

		constexpr iterator cend( ) const {
			return iterator{};
		}
	};

	template<typename CharT, typename Delemiter>
	string_split_range( daw::basic_string_view<CharT>, Delemiter && )
	  ->string_split_range<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, typename Delemiter>
	string_split_range( daw::basic_string_view<CharT>, Delemiter const & )
	  ->string_split_range<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, size_t N>
	string_split_range( daw::basic_string_view<CharT>, CharT const ( & )[N] )
	  ->string_split_range<CharT, daw::basic_string_view<CharT>>;

	template<typename CharT, typename Delemiter>
	string_split_range( std::basic_string<CharT>, Delemiter && )
	  ->string_split_range<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, typename Delemiter>
	string_split_range( std::basic_string<CharT>, Delemiter const & )
	  ->string_split_range<CharT, std::remove_reference_t<Delemiter>>;

	template<typename CharT, size_t N>
	string_split_range( std::basic_string<CharT>, CharT const ( & )[N] )
	  ->string_split_range<CharT, daw::basic_string_view<CharT>>;

	template<typename CharT, typename Delemiter>
	auto split_string( std::basic_string<CharT> const &str,
	                   Delemiter &&delemiter ) noexcept {
		return string_split_range( str, std::forward<Delemiter>( delemiter ) );
	}

	template<typename CharT, typename Delemiter>
	constexpr auto split_string( daw::basic_string_view<CharT> str,
	                             Delemiter &&delemiter ) noexcept {

		return string_split_range( str, std::forward<Delemiter>( delemiter ) );
	}

	template<typename CharT, size_t N, typename Delemiter>
	constexpr auto split_string( CharT const ( &str )[N],
	                             Delemiter &&delemiter ) noexcept {

		daw::basic_string_view<CharT, std::char_traits<CharT>, N> sv( str, N );
		return string_split_range( sv, std::forward<Delemiter>( delemiter ) );
	}
} // namespace daw

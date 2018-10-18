// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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
#include "daw_string_view.h"
#include "daw_traits.h"
#include "iterator/daw_reverse_iterator.h"

namespace daw {
	template<typename CharT>
	class string_split_iterator {
		static constexpr size_t const npos = std::basic_string<CharT>::npos;

		daw::basic_string_view<CharT> m_str{};
		std::basic_string<CharT> m_delemiter{};
		size_t m_pos{};

		size_t find_next( ) const noexcept {
			if( m_str.empty( ) ) {
				return npos;
			}
			return m_str.find( daw::make_string_view( m_delemiter ), m_pos );
		}

		void move_next( ) {
			auto next_pos = find_next( );
			if( next_pos == npos ) {
				m_pos = npos;
				return;
			}
			m_pos = next_pos + m_delemiter.size( );
		}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::basic_string_view<CharT>;
		using reference = daw::basic_string_view<CharT>;
		using pointer = daw::basic_string_view<CharT>;
		using difference_type = ptrdiff_t;

		string_split_iterator( ) noexcept = default;

		string_split_iterator( daw::basic_string_view<CharT> str,
		                       std::basic_string<CharT> delemiter ) noexcept
		  : m_str{str}
		  , m_delemiter{std::move( delemiter )}
		  , m_pos{0} {}

		string_split_iterator( std::basic_string<CharT> const &str,
		                       std::basic_string<CharT> delemiter ) noexcept
		  : m_str{make_string_view( str )}
		  , m_delemiter{std::move( delemiter )}
		  , m_pos{0} {}

		string_split_iterator &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		string_split_iterator operator++( int ) const noexcept {
			string_split_iterator tmp{*this};
			tmp.move_next( );
			return tmp;
		}

		daw::basic_string_view<CharT> operator*( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		daw::basic_string_view<CharT> operator->( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		friend bool operator==( string_split_iterator const &lhs,
		                        string_split_iterator const &rhs ) noexcept {
			if( ( lhs.m_pos == npos and npos == rhs.m_pos ) ) {
				return true;
			}
			return std::tie( lhs.m_str, lhs.m_pos, lhs.m_delemiter ) ==
			       std::tie( rhs.m_str, rhs.m_pos, rhs.m_delemiter );
		}

		friend bool operator!=( string_split_iterator const &lhs,
		                        string_split_iterator const &rhs ) noexcept {
			if( ( lhs.m_pos == npos and npos == rhs.m_pos ) ) {
				return false;
			}
			return std::tie( lhs.m_str, lhs.m_pos, lhs.m_delemiter ) !=
			       std::tie( rhs.m_str, rhs.m_pos, rhs.m_delemiter );
		}
	};

	template<typename CharT>
	struct string_split_range {
		using iterator = string_split_iterator<CharT>;

	private:
		iterator m_first{};

	public:
		string_split_range( ) noexcept = default;

		string_split_range( daw::basic_string_view<CharT> str,
		                    std::string delemiter ) noexcept
		  : m_first{str, std::move( delemiter )} {}

		string_split_range( std::basic_string<CharT> const &str,
		                    std::string delemiter ) noexcept
		  : m_first{str, std::move( delemiter )} {}

		iterator begin( ) const {
			return m_first;
		}

		iterator cbegin( ) const {
			return m_first;
		}

		iterator end( ) const {
			return iterator{};
		}

		iterator cend( ) const {
			return iterator{};
		}
	};

	template<typename CharT, typename Delemiter>
	auto split_string( std::basic_string<CharT> const &str,
	                   Delemiter delemiter ) noexcept {

		std::basic_string<CharT> d = delemiter;
		return string_split_range<CharT>{str, std::move( d )};
	}

	template<typename CharT, typename Delemiter>
	auto split_string( daw::basic_string_view<CharT> str,
	                   Delemiter delemiter ) noexcept {

		std::basic_string<CharT> d = delemiter;
		return string_split_range<CharT>{std::move( str ), std::move( d )};
	}

	template<typename CharT, size_t N, typename Delemiter>
	auto split_string( CharT const ( &str )[N], Delemiter delemiter ) noexcept {

		daw::basic_string_view<CharT> sv{str};
		std::basic_string<CharT> d = delemiter;
		return string_split_range<CharT>{std::move( sv ), std::move( d )};
	}

} // namespace daw

// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		struct splitter_size {
			template<typename T, std::enable_if_t<!daw::traits::has_size_memberfn_v<T>, std::nullptr_t> = nullptr>
			constexpr size_t operator( )( T const & ) noexcept {
				return 1;
			}

			template<typename T, std::enable_if_t<daw::traits::has_size_memberfn_v<T>, std::nullptr_t> = nullptr>
			constexpr size_t operator( )( T const &t ) noexcept( noexcept( t.size( ) ) ) {
				return t.size( );
			}
		};
	} // namespace impl

	template<typename CharT>
	struct char_splitter_t {
		CharT c;
		constexpr bool operator( )( CharT rhs ) noexcept {
			return c == rhs;
		}
	};

	template<typename Iterator, typename CharT = typename std::iterator_traits<Iterator>::value_type,
	         typename Splitter = char_splitter_t<CharT>>
	struct split_it {
		static_assert( daw::is_unary_predicate_v<Splitter, CharT>,
		               "Splitter does not fullfill the roll of a Unary Predicate that takes a CharT as it's argument" );
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = daw::basic_string_view<CharT>;
		using difference_type = std::ptrdiff_t;
		using pointer = CharT const *;
		using reference = CharT const &;

	private:
		Iterator m_iterator;
		Splitter m_splitter;

		constexpr size_t find_prev( ) noexcept {
			auto pos = m_data.substr( 0, m_last ).find_last_of_if( m_splitter );
			if( pos == m_data.npos ) {
				return 0;
			}
			return pos + impl::splitter_size{}( m_splitter );
		}

		constexpr void move_prev( ) noexcept {
			if( m_first == 0 ) {
				return;
			}
			m_last = m_first - impl::splitter_size{}( m_splitter );
			m_first = find_prev( );
		}

		constexpr size_t find_next( ) noexcept {
			if( m_last == m_data.npos ) {
				return m_last;
			}
			return m_data.find_first_of_if( m_splitter, m_last + impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_last != m_data.npos ) {
				m_last += impl::splitter_size{}( m_splitter );
			}
			m_first = m_last;
			m_last = find_next( );
		}

	public:
		template<typename String, typename SplitterArg>
		constexpr split_it( String const &str, SplitterArg &&splitter_arg ) noexcept
		  : m_data{daw::make_string_view_it( std::cbegin( str ), std::cend( str ) )}
		  , m_splitter{std::forward<SplitterArg>( splitter_arg )}
		  , m_first{0}
		  , m_last{0} {

			m_last = find_next( );
		}

		constexpr split_it( ) noexcept
		  : m_data{}
		  , m_splitter{}
		  , m_first{m_data.npos}
		  , m_last{m_data.npos} {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{other.m_data}
		  , m_splitter{other.m_splitter}
		  , m_first{other.m_first}
		  , m_last{other.m_last} {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{std::move( other.m_data )}
		  , m_splitter{std::move( other.m_splitter )}
		  , m_first{std::move( other.m_first )}
		  , m_last{std::move( other.m_last )} {}

		constexpr split_it &operator=( split_it const &rhs ) noexcept {
			if( this != &rhs ) {
				m_data = rhs.m_data;
				m_splitter = rhs.m_splitter;
				m_first = rhs.m_first;
				m_last = rhs.m_last;
			}
			return *this;
		}

		constexpr split_it &operator=( split_it &&rhs ) noexcept {
			if( this != &rhs ) {
				m_data = std::move( rhs.m_data );
				m_splitter = std::move( rhs.m_splitter );
				m_first = std::move( rhs.m_first );
				m_last = std::move( rhs.m_last );
			}
			return *this;
		}

		template<typename String, std::enable_if_t<daw::is_convertible_v<String, value_type>, std::nullptr_t> = nullptr>
		constexpr split_it &operator=( String const &str ) noexcept {
			m_data = value_type{str};
			m_first = 0;
			m_last = 0;
			m_last = find_next( );
			return *this;
		}

		constexpr split_it &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr split_it &operator--( ) noexcept {
			move_prev( );
			return *this;
		}

		constexpr split_it operator++(int)const noexcept {
			split_it tmp{*this};
			tmp.move_next( );
			return tmp;
		}

		constexpr split_it operator--(int)const noexcept {
			split_it tmp{*this};
			tmp.move_prev( );
			return tmp;
		}

		constexpr split_it &operator+=( std::ptrdiff_t n ) {
			for( ; n > 0; --n ) {
				move_next( );
			}
			return *this;
		}

		constexpr split_it &operator-=( std::ptrdiff_t n ) {
			for( ; n > 0; --n ) {
				move_prev( );
			}
			return *this;
		}

		constexpr split_it operator+( std::ptrdiff_t n ) noexcept {
			split_it tmp{*this};
			tmp += n;
			return tmp;
		}

		constexpr split_it operator-( std::ptrdiff_t n ) noexcept {
			split_it tmp{*this};
			tmp -= n;
			return tmp;
		}

		constexpr value_type operator*( ) const noexcept {
			return m_data.substr( m_first, m_last - m_first );
		}

		constexpr value_type operator->( ) const noexcept {
			return m_data.substr( m_first, m_last - m_first );
		}

		constexpr friend bool operator==( split_it const &lhs, split_it const &rhs ) noexcept {
			if( lhs.at_end( ) != rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_first, lhs.m_last, lhs.m_data ) == std::tie( rhs.m_first, rhs.m_last, rhs.m_data );
		}

		constexpr friend bool operator!=( split_it const &lhs, split_it const &rhs ) noexcept {
			if( lhs.at_end( ) == rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_first, lhs.m_last, lhs.m_data ) != std::tie( rhs.m_first, rhs.m_last, rhs.m_data );
		}

		static constexpr split_it make_end( ) noexcept {
			return split_it<CharT>{};
		}

		constexpr bool at_end( ) const noexcept {
			return m_first >= m_data.size( );
		}
	};

	template<typename String, typename CharT>
	constexpr auto make_split_it( String const &sv, CharT divider ) noexcept {
		using value_t = std::decay_t<decltype( *std::cbegin( sv ) )>;
		auto result = split_it<value_t>( sv, divider );
		return result;
	}
} // namespace daw


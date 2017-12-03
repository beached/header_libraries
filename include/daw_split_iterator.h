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

#include "daw_algorithm.h"
#include "daw_reverse_iterator.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		struct splitter_size {
			template<typename T, daw::required<!daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr size_t operator( )( T const & ) noexcept {
				return 1;
			}

			template<typename T, daw::required<daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr size_t operator( )( T const &t ) noexcept( noexcept( t.size( ) ) ) {
				return t.size( );
			}
		};

		template<typename Iterator>
		constexpr bool can_decrement = daw::is_detected_v<decltype( --std::declval<Iterator>( ) )>;
	} // namespace impl

	template<typename CharT>
	struct char_splitter_t {
		CharT c;
		constexpr char_splitter_t( CharT val ) noexcept
		  : c{val} {}

		constexpr bool operator( )( CharT rhs ) noexcept {
			return c == rhs;
		}
	};

	template<typename Iterator, typename Splitter = char_splitter_t<typename std::iterator_traits<Iterator>::value_type>>
	struct split_it {
		using CharT = typename std::iterator_traits<Iterator>::value_type;
		static_assert( daw::is_unary_predicate_v<Splitter, CharT>,
		               "Splitter does not fullfill the roll of a Unary Predicate that takes a CharT as it's argument" );
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = daw::basic_string_view<CharT>;
		using difference_type = std::ptrdiff_t;
		using pointer = CharT const *;
		using reference = CharT const &;

	private:
		struct range_t {
			Iterator m_first;
			Iterator m_last;

			Iterator const begin( ) const {
				return m_first;
			}

			Iterator const end( ) const {
				return m_last;
			}
		} m_data;

		Iterator m_first;
		Iterator m_last;
		Splitter m_splitter;

		constexpr Iterator find_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
			auto pos = daw::algorithm::find_last_of( m_data.begin( ), m_last, m_splitter );
			if( pos == m_data.end( ) ) {
				return 0;
			}
			return daw::algorithm::safe_next( pos, m_data.end( ), impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
			if( m_first == m_data.begin( ) ) {
				return;
			}
			m_last = daw::algorithm::safe_prev( m_first, m_data.begin( ), impl::splitter_size{}( m_splitter ) );
			m_first = find_prev( );
		}

		constexpr Iterator find_next( ) noexcept {
			if( m_last == m_data.end( ) ) {
				return m_last;
			}
			return daw::algorithm::find_first_of( daw::algorithm::safe_next( m_last, m_data.end( ) ), m_data.end( ),
			                                      impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_last != m_data.end( ) ) {
				daw::algorithm::advance( m_last, impl::splitter_size{}( m_splitter ) );
			}
			m_first = m_last;
			m_last = find_next( );
		}

	public:
		constexpr split_it( Iterator first, Iterator last, Splitter &&splitter ) noexcept
		  : m_data{first, last}
		  , m_first{first}
		  , m_last{last}
		  , m_splitter{std::forward<Splitter>( splitter )} {

			m_last = find_next( );
		}

		template<typename Container, daw::required<daw::traits::is_container_like_v<Container>> = nullptr>
		constexpr split_it( Container &container, Splitter &&splitter ) noexcept
		  : split_it( std::begin( container ), std::end( container ), std::forward<Splitter>( splitter ) ) {}

		constexpr split_it( ) noexcept
		  : m_data{{}, {}}
		  , m_first{}
		  , m_last{}
		  , m_splitter{} {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{other.m_data}
		  , m_first{other.m_first}
		  , m_last{other.m_last}
		  , m_splitter{other.m_splitter} {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{std::move( other.m_data )}
		  , m_first{std::move( other.m_first )}
		  , m_last{std::move( other.m_last )}
		  , m_splitter{std::move( other.m_splitter )} {}

		constexpr split_it &operator=( split_it const &rhs ) noexcept {
			if( this != &rhs ) {
				m_data = rhs.m_data;
				m_first = rhs.m_first;
				m_last = rhs.m_last;
				m_splitter = rhs.m_splitter;
			}
			return *this;
		}

		constexpr split_it &operator=( split_it &&rhs ) noexcept {
			if( this != &rhs ) {
				m_data = std::move( rhs.m_data );
				m_first = std::move( rhs.m_first );
				m_last = std::move( rhs.m_last );
				m_splitter = std::move( rhs.m_splitter );
			}
			return *this;
		}

		constexpr split_it &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr split_it &operator--( ) noexcept {
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
			move_prev( );
			return *this;
		}

		constexpr split_it operator++(int)const noexcept {
			split_it tmp{*this};
			tmp.move_next( );
			return tmp;
		}

		constexpr split_it operator--(int)const noexcept {
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
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
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
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
			static_assert( impl::can_decrement<Iterator>, "Supplied Iterator is not Bidirectional" );
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

		constexpr friend bool operator==( split_it const &lhs, Iterator const &rhs ) noexcept {
			return lhs.m_first == rhs;
		}

		constexpr friend bool operator==( Iterator const &lhs, split_it const &rhs ) noexcept {
			return lhs == rhs.m_first;
		}

		constexpr friend bool operator!=( split_it const &lhs, split_it const &rhs ) noexcept {
			if( lhs.at_end( ) == rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_first, lhs.m_last, lhs.m_data ) != std::tie( rhs.m_first, rhs.m_last, rhs.m_data );
		}

		constexpr friend bool operator!=( split_it const &lhs, Iterator const &rhs ) noexcept {
			return lhs.m_first != rhs;
		}

		constexpr friend bool operator!=( Iterator const &lhs, split_it const &rhs ) noexcept {
			return lhs == rhs.m_first;
		}

		static constexpr split_it make_end( ) noexcept {
			return split_it{};
		}

		constexpr bool at_end( ) const noexcept {
			return m_first == m_data.end( );
		}
	};
	namespace impl {
		template<typename String>
		using string_char_t = std::decay_t<decltype( *std::cbegin( std::declval<String>( ) ) )>;

		template<typename Splitter, typename String>
		constexpr bool is_splitter_v = daw::is_unary_predicate_v<Splitter, string_char_t<String>>;
	} // namespace impl

	template<typename String, typename Splitter, daw::required<impl::is_splitter_v<Splitter, String>> = nullptr>
	constexpr auto make_split_it( String &sv, Splitter &&splitter ) noexcept {
		using IterT = decltype( std::begin( sv ) );
		auto result = split_it<IterT, Splitter>{std::begin( sv ), std::end( sv ), std::forward<Splitter>( splitter )};
		return result;
	}

	template<typename String, typename CharT, daw::required<!impl::is_splitter_v<CharT, String>> = nullptr>
	constexpr auto make_split_it( String const &sv, CharT divider ) noexcept {
		using IterT = decltype( std::begin( sv ) );
		auto result =
		  split_it<IterT, char_splitter_t<CharT>>{std::begin( sv ), std::end( sv ), char_splitter_t<CharT>{divider}};
		return result;
	}
} // namespace daw


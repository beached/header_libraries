// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include "daw_algorithm.h"
#include "daw_reverse_iterator.h"
#include "daw_traits.h"
#include <string>

namespace daw {
	namespace impl {
		struct splitter_size {
			template<typename T,
			         daw::required<!daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr size_t operator( )( T const & ) noexcept {
				return 1;
			}

			template<typename T,
			         daw::required<daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr size_t
			operator( )( T const &t ) noexcept( noexcept( t.size( ) ) ) {
				return t.size( );
			}
		};

		template<typename Iterator>
		constexpr bool can_decrement =
		  daw::is_detected_v<decltype( --std::declval<Iterator>( ) )>;
	} // namespace impl

	template<typename CharT>
	struct char_splitter_t {
		CharT c;

		constexpr bool operator( )( CharT rhs ) const noexcept {
			return c == rhs;
		}
	};

	template<typename Iterator>
	struct splitter_range_t {
		Iterator m_first;
		Iterator m_last;

		Iterator const cbegin( ) const noexcept {
			return m_first;
		}

		constexpr Iterator const begin( ) const noexcept {
			return m_first;
		}

		constexpr Iterator begin( ) noexcept {
			return m_first;
		}

		constexpr Iterator const cend( ) const noexcept {
			return m_last;
		}

		constexpr Iterator const end( ) const noexcept {
			return m_last;
		}

		constexpr Iterator end( ) noexcept {
			return m_last;
		}
	};

	template<typename Iterator>
	constexpr bool operator==( splitter_range_t<Iterator> const &lhs,
	                           splitter_range_t<Iterator> const &rhs ) noexcept {
		return lhs.m_first == rhs.m_first && lhs.m_last == rhs.m_last;
	}

	template<typename Iterator>
	constexpr bool operator!=( splitter_range_t<Iterator> const &lhs,
	                           splitter_range_t<Iterator> const &rhs ) noexcept {
		return lhs.m_first != rhs.m_first || lhs.m_last != rhs.m_last;
	}

	template<typename Iterator, typename Splitter, typename...>
	struct split_it;

	template<typename Iterator, typename Splitter>
	struct split_it<
	  Iterator, Splitter,
	  std::enable_if_t<
	    !is_same_v<std::decay_t<decltype( *std::declval<Iterator>( ) )>, char>>> {
		using CharT = typename std::iterator_traits<Iterator>::value_type;
		static_assert( daw::is_unary_predicate_v<Splitter, CharT>,
		               "Splitter does not fullfill the roll of a Unary Predicate "
		               "that takes a CharT as it's argument" );
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type = ptrdiff_t;
		using pointer = CharT const *;
		using reference = CharT const &;

	private:
		splitter_range_t<Iterator> m_data;
		splitter_range_t<Iterator> m_position;
		Splitter m_splitter;

		constexpr Iterator find_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			auto pos = daw::algorithm::find_last_of( m_data.cbegin( ),
			                                         m_position.cend( ), m_splitter );
			if( pos == m_data.cend( ) ) {
				return 0;
			}
			return daw::safe_next( pos, m_data.cend( ),
			                       impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			if( m_position.cbegin( ) == m_data.cbegin( ) ) {
				return;
			}
			m_position.end( ) =
			  daw::safe_prev( m_position.cbegin( ), m_data.cbegin( ),
			                  impl::splitter_size{}( m_splitter ) );
			m_position.begin( ) = find_prev( );
		}

		constexpr Iterator find_next( ) noexcept {
			if( m_position.end( ) == m_data.cend( ) ) {
				return m_position.end( );
			}
			return daw::algorithm::find_first_of(
			  daw::safe_next( m_position.end( ), m_data.cend( ) ), m_data.cend( ),
			  impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_position.end( ) != m_data.cend( ) ) {
				daw::advance( m_position.end( ), impl::splitter_size{}( m_splitter ) );
			}
			m_position.begin( ) = m_position.end( );
			m_position.end( ) = find_next( );
		}

	public:
		constexpr split_it( Iterator first, Iterator last,
		                    Splitter &&splitter ) noexcept
		  : m_data{first, last}
		  , m_position{first, last}
		  , m_splitter{std::forward<Splitter>( splitter )} {

			m_position.end( ) = find_next( );
		}

		template<
		  typename Container,
		  daw::required<daw::traits::is_container_like_v<Container>> = nullptr>
		constexpr split_it( Container &container, Splitter &&splitter ) noexcept
		  : split_it( std::begin( container ), std::end( container ),
		              std::forward<Splitter>( splitter ) ) {}

		constexpr split_it( ) noexcept
		  : m_data{{}, {}}
		  , m_position{{}, {}}
		  , m_splitter{} {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{other.m_data}
		  , m_position{other.m_position}
		  , m_splitter{other.m_splitter} {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{std::move( other.m_data )}
		  , m_position{std::move( other.m_position )}
		  , m_splitter{std::move( other.m_splitter )} {}

		constexpr split_it &operator=( split_it const &rhs ) noexcept {
			if( this != &rhs ) {
				m_data = rhs.m_data;
				m_position = rhs.m_position;
				m_splitter = rhs.m_splitter;
			}
			return *this;
		}

		constexpr split_it &operator=( split_it &&rhs ) noexcept {
			if( this != &rhs ) {
				m_data = std::move( rhs.m_data );
				m_position = std::move( rhs.m_position );
				m_splitter = std::move( rhs.m_splitter );
			}
			return *this;
		}

		constexpr split_it &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr split_it &operator--( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			move_prev( );
			return *this;
		}

		constexpr split_it operator++( int ) const noexcept {
			split_it tmp{*this};
			move_next( );
			return tmp;
		}

		constexpr split_it operator--( int ) const noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{*this};
			move_prev( );
			return tmp;
		}

		constexpr split_it &operator+=( ptrdiff_t n ) {
			for( ; n > 0; --n ) {
				move_next( );
			}
			return *this;
		}

		constexpr split_it &operator-=( ptrdiff_t n ) {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			for( ; n > 0; --n ) {
				move_prev( );
			}
			return *this;
		}

		constexpr split_it operator+( ptrdiff_t n ) noexcept {
			split_it tmp{*this};
			for( ; n > 0; --n ) {
				move_next( );
			}
			return tmp;
		}

		constexpr split_it operator-( ptrdiff_t n ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{*this};
			for( ; n > 0; --n ) {
				move_prev( );
			}
			return tmp;
		}

		constexpr splitter_range_t<Iterator> operator*( ) const noexcept {
			return m_position;
		}

		constexpr splitter_range_t<Iterator> operator->( ) const noexcept {
			return m_position;
		}

		constexpr friend bool operator==( split_it const &lhs,
		                                  split_it const &rhs ) noexcept {
			if( lhs.at_end( ) != rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_position, lhs.m_data ) ==
			       std::tie( rhs.m_position, rhs.m_data );
		}

		constexpr friend bool operator==( split_it const &lhs,
		                                  Iterator const &rhs ) noexcept {
			return lhs.m_position.begin( ) == rhs;
		}

		constexpr friend bool operator==( Iterator const &lhs,
		                                  split_it const &rhs ) noexcept {
			return lhs == rhs.m_position.begin( );
		}

		constexpr friend bool operator!=( split_it const &lhs,
		                                  split_it const &rhs ) noexcept {
			if( lhs.at_end( ) == rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_position.begin( ), lhs.m_position.end( ),
			                 lhs.m_data ) != std::tie( rhs.m_position.begin( ),
			                                           rhs.m_position.end( ),
			                                           rhs.m_data );
		}

		constexpr friend bool operator!=( split_it const &lhs,
		                                  Iterator const &rhs ) noexcept {
			return lhs.m_position.begin( ) != rhs;
		}

		constexpr friend bool operator!=( Iterator const &lhs,
		                                  split_it const &rhs ) noexcept {
			return lhs == rhs.m_position.begin( );
		}

		static constexpr split_it make_end( ) noexcept {
			return split_it{};
		}

		constexpr bool at_end( ) const noexcept {
			return m_position.begin( ) == m_data.cend( );
		}
	};

	template<typename Iterator, typename Splitter>
	struct split_it<
	  Iterator, Splitter,
	  std::enable_if_t<
	    is_same_v<std::decay_t<decltype( *std::declval<Iterator>( ) )>, char>>> {
		using CharT = typename std::iterator_traits<Iterator>::value_type;
		static_assert( daw::is_unary_predicate_v<Splitter, CharT>,
		               "Splitter does not fullfill the roll of a Unary Predicate "
		               "that takes a CharT as it's argument" );
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type = ptrdiff_t;
		using pointer = CharT const *;
		using reference = CharT const &;

	private:
		splitter_range_t<Iterator> m_data;
		splitter_range_t<Iterator> m_position;
		Splitter m_splitter;

		constexpr Iterator find_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			auto pos = daw::algorithm::find_last_of( m_data.cbegin( ),
			                                         m_position.cend( ), m_splitter );
			if( pos == m_data.cend( ) ) {
				return 0;
			}
			return daw::safe_next( pos, m_data.cend( ),
			                       impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			if( m_position.cbegin( ) == m_data.cbegin( ) ) {
				return;
			}
			m_position.end( ) =
			  daw::safe_prev( m_position.cbegin( ), m_data.cbegin( ),
			                  impl::splitter_size{}( m_splitter ) );
			m_position.begin( ) = find_prev( );
		}

		constexpr Iterator find_next( ) noexcept {
			if( m_position.end( ) == m_data.cend( ) ) {
				return m_position.end( );
			}
			return daw::algorithm::find_first_of(
			  daw::safe_next( m_position.end( ), m_data.cend( ) ), m_data.cend( ),
			  impl::splitter_size{}( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_position.end( ) != m_data.cend( ) ) {
				daw::advance( m_position.end( ), impl::splitter_size{}( m_splitter ) );
			}
			m_position.begin( ) = m_position.end( );
			m_position.end( ) = find_next( );
		}

	public:
		constexpr split_it( Iterator first, Iterator last,
		                    Splitter &&splitter ) noexcept
		  : m_data{first, last}
		  , m_position{first, last}
		  , m_splitter{std::forward<Splitter>( splitter )} {

			m_position.end( ) = find_next( );
		}

		template<
		  typename Container,
		  daw::required<daw::traits::is_container_like_v<Container>> = nullptr>
		constexpr split_it( Container &container, Splitter &&splitter ) noexcept
		  : split_it( std::begin( container ), std::end( container ),
		              std::forward<Splitter>( splitter ) ) {}

		constexpr split_it( ) noexcept
		  : m_data{{}, {}}
		  , m_position{{}, {}}
		  , m_splitter{} {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{other.m_data}
		  , m_position{other.m_position}
		  , m_splitter{other.m_splitter} {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{std::move( other.m_data )}
		  , m_position{std::move( other.m_position )}
		  , m_splitter{std::move( other.m_splitter )} {}

		constexpr split_it &operator=( split_it const &rhs ) noexcept {
			if( this != &rhs ) {
				m_data = rhs.m_data;
				m_position = rhs.m_position;
				m_splitter = rhs.m_splitter;
			}
			return *this;
		}

		constexpr split_it &operator=( split_it &&rhs ) noexcept {
			if( this != &rhs ) {
				m_data = std::move( rhs.m_data );
				m_position = std::move( rhs.m_position );
				m_splitter = std::move( rhs.m_splitter );
			}
			return *this;
		}

		constexpr split_it &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr split_it &operator--( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			move_prev( );
			return *this;
		}

		constexpr split_it operator++( int ) const noexcept {
			split_it tmp{*this};
			move_next( );
			return tmp;
		}

		constexpr split_it operator--( int ) const noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{*this};
			move_prev( );
			return tmp;
		}

		constexpr split_it &operator+=( ptrdiff_t n ) {
			for( ; n > 0; --n ) {
				move_next( );
			}
			return *this;
		}

		constexpr split_it &operator-=( ptrdiff_t n ) {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			for( ; n > 0; --n ) {
				move_prev( );
			}
			return *this;
		}

		constexpr split_it operator+( ptrdiff_t n ) noexcept {
			split_it tmp{*this};
			for( ; n > 0; --n ) {
				move_next( );
			}
			return tmp;
		}

		constexpr split_it operator-( ptrdiff_t n ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{*this};
			for( ; n > 0; --n ) {
				move_prev( );
			}
			return tmp;
		}

		constexpr splitter_range_t<Iterator> operator*( ) const noexcept {
			return m_position;
		}

		constexpr splitter_range_t<Iterator> operator->( ) const noexcept {
			return m_position;
		}

		constexpr friend bool operator==( split_it const &lhs,
		                                  split_it const &rhs ) noexcept {
			if( lhs.at_end( ) != rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_position, lhs.m_data ) ==
			       std::tie( rhs.m_position, rhs.m_data );
		}

		constexpr friend bool operator==( split_it const &lhs,
		                                  Iterator const &rhs ) noexcept {
			return lhs.m_position.begin( ) == rhs;
		}

		constexpr friend bool operator==( Iterator const &lhs,
		                                  split_it const &rhs ) noexcept {
			return lhs == rhs.m_position.begin( );
		}

		constexpr friend bool operator!=( split_it const &lhs,
		                                  split_it const &rhs ) noexcept {
			if( lhs.at_end( ) == rhs.at_end( ) ) {
				return false;
			}
			return std::tie( lhs.m_position.begin( ), lhs.m_position.end( ),
			                 lhs.m_data ) != std::tie( rhs.m_position.begin( ),
			                                           rhs.m_position.end( ),
			                                           rhs.m_data );
		}

		constexpr friend bool operator!=( split_it const &lhs,
		                                  Iterator const &rhs ) noexcept {
			return lhs.m_position.begin( ) != rhs;
		}

		constexpr friend bool operator!=( Iterator const &lhs,
		                                  split_it const &rhs ) noexcept {
			return lhs == rhs.m_position.begin( );
		}

		static constexpr split_it make_end( ) noexcept {
			return split_it{};
		}

		constexpr bool at_end( ) const noexcept {
			return m_position.begin( ) == m_data.cend( );
		}
	};

	namespace impl {
		template<typename String>
		using string_char_t =
		  std::decay_t<decltype( *std::cbegin( std::declval<String>( ) ) )>;

		template<typename Splitter, typename String>
		constexpr bool is_splitter_v =
		  daw::is_unary_predicate_v<Splitter, string_char_t<String>>;
	} // namespace impl

	template<typename String, typename Splitter,
	         daw::required<impl::is_splitter_v<Splitter, String>> = nullptr>
	constexpr auto make_split_it( String &sv, Splitter &&splitter ) noexcept {
		using IterT = decltype( std::begin( sv ) );
		auto result = split_it<IterT, Splitter>{std::begin( sv ), std::end( sv ),
		                                        std::forward<Splitter>( splitter )};
		return result;
	}

	template<typename CharT, typename Traits, typename Allocator>
	constexpr auto
	make_split_it( std::basic_string<CharT, Traits, Allocator> &str,
	               CharT divider ) noexcept {
		using SpltT = char_splitter_t<CharT>;
		using iterator =
		  typename std::basic_string<CharT, Traits, Allocator>::iterator;

		return split_it<iterator, SpltT>( str.begin( ), str.end( ),
		                                  SpltT{std::move( divider )} );
	}
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"
#include "../daw_algorithm.h"
#include "../daw_move.h"
#include "../daw_traits.h"
#include "daw_reverse_iterator.h"

#include <cstddef>
#include <string>

namespace daw {
	namespace impl {
		struct splitter_size {
			template<typename T,
			         daw::required<not daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr std::size_t operator( )( T const & ) noexcept {
				return 1;
			}

			template<typename T,
			         daw::required<daw::traits::has_size_memberfn_v<T>> = nullptr>
			constexpr std::size_t
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
		return lhs.m_first == rhs.m_first and lhs.m_last == rhs.m_last;
	}

	template<typename Iterator>
	constexpr bool operator!=( splitter_range_t<Iterator> const &lhs,
	                           splitter_range_t<Iterator> const &rhs ) noexcept {
		return lhs.m_first != rhs.m_first or lhs.m_last != rhs.m_last;
	}

	template<typename Iterator, typename Splitter, typename...>
	struct split_it;

	template<typename Iterator, typename Splitter>
	struct split_it<
	  Iterator, Splitter,
	  std::enable_if_t<not std::is_same_v<
	    char, typename std::iterator_traits<Iterator>::value_type>>> {

		using CharT = typename std::iterator_traits<Iterator>::value_type;
		static_assert( daw::traits::is_unary_predicate_v<Splitter, CharT>,
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
			                       impl::splitter_size{ }( m_splitter ) );
		}

		constexpr void move_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			if( m_position.cbegin( ) == m_data.cbegin( ) ) {
				return;
			}
			m_position.end( ) =
			  daw::safe_prev( m_position.cbegin( ), m_data.cbegin( ),
			                  impl::splitter_size{ }( m_splitter ) );
			m_position.begin( ) = find_prev( );
		}

		constexpr Iterator find_next( ) noexcept {
			if( m_position.end( ) == m_data.cend( ) ) {
				return m_position.end( );
			}
			return daw::algorithm::find_first_of(
			  daw::safe_next( m_position.end( ), m_data.cend( ) ), m_data.cend( ),
			  impl::splitter_size{ }( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_position.end( ) != m_data.cend( ) ) {
				daw::advance( m_position.end( ), impl::splitter_size{ }( m_splitter ) );
			}
			m_position.begin( ) = m_position.end( );
			m_position.end( ) = find_next( );
		}

	public:
		constexpr split_it( Iterator first, Iterator last,
		                    Splitter &&splitter ) noexcept
		  : m_data{ first, last }
		  , m_position{ first, last }
		  , m_splitter{ std::forward<Splitter>( splitter ) } {

			m_position.end( ) = find_next( );
		}

		template<
		  typename Container,
		  daw::required<daw::traits::is_container_like_v<Container>> = nullptr>
		constexpr split_it( Container &container, Splitter &&splitter ) noexcept
		  : split_it( std::begin( container ), std::end( container ),
		              std::forward<Splitter>( splitter ) ) {}

		constexpr split_it( ) noexcept
		  : m_data{ { }, {} }
		  , m_position{ { }, {} }
		  , m_splitter{ } {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{ other.m_data }
		  , m_position{ other.m_position }
		  , m_splitter{ other.m_splitter } {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{ DAW_MOVE( other.m_data ) }
		  , m_position{ DAW_MOVE( other.m_position ) }
		  , m_splitter{ DAW_MOVE( other.m_splitter ) } {}

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
				m_data = DAW_MOVE( rhs.m_data );
				m_position = DAW_MOVE( rhs.m_position );
				m_splitter = DAW_MOVE( rhs.m_splitter );
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
			split_it tmp{ *this };
			move_next( );
			return tmp;
		}

		constexpr split_it operator--( int ) const noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{ *this };
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
			split_it tmp{ *this };
			for( ; n > 0; --n ) {
				move_next( );
			}
			return tmp;
		}

		constexpr split_it operator-( ptrdiff_t n ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{ *this };
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
			return split_it{ };
		}

		constexpr bool at_end( ) const noexcept {
			return m_position.begin( ) == m_data.cend( );
		}
	};

	template<typename Iterator, typename Splitter>
	struct split_it<
	  Iterator, Splitter,
	  std::enable_if_t<std::is_same_v<
	    char, typename std::iterator_traits<Iterator>::value_type>>> {

		using CharT = char;
		static_assert( daw::traits::is_unary_predicate_v<Splitter, CharT>,
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
			                       impl::splitter_size{ }( m_splitter ) );
		}

		constexpr void move_prev( ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			if( m_position.cbegin( ) == m_data.cbegin( ) ) {
				return;
			}
			m_position.end( ) =
			  daw::safe_prev( m_position.cbegin( ), m_data.cbegin( ),
			                  impl::splitter_size{ }( m_splitter ) );
			m_position.begin( ) = find_prev( );
		}

		constexpr Iterator find_next( ) noexcept {
			if( m_position.end( ) == m_data.cend( ) ) {
				return m_position.end( );
			}
			return daw::algorithm::find_first_of(
			  daw::safe_next( m_position.end( ), m_data.cend( ) ), m_data.cend( ),
			  impl::splitter_size{ }( m_splitter ) );
		}

		constexpr void move_next( ) noexcept {
			if( m_position.end( ) != m_data.cend( ) ) {
				daw::advance( m_position.end( ), impl::splitter_size{ }( m_splitter ) );
			}
			m_position.begin( ) = m_position.end( );
			m_position.end( ) = find_next( );
		}

	public:
		constexpr split_it( Iterator first, Iterator last,
		                    Splitter &&splitter ) noexcept
		  : m_data{ first, last }
		  , m_position{ first, last }
		  , m_splitter{ std::forward<Splitter>( splitter ) } {

			m_position.end( ) = find_next( );
		}

		template<
		  typename Container,
		  daw::required<daw::traits::is_container_like_v<Container>> = nullptr>
		constexpr split_it( Container &container, Splitter &&splitter ) noexcept
		  : split_it( std::begin( container ), std::end( container ),
		              std::forward<Splitter>( splitter ) ) {}

		constexpr split_it( ) noexcept
		  : m_data{ { }, {} }
		  , m_position{ { }, {} }
		  , m_splitter{ } {}

		~split_it( ) noexcept = default;

		constexpr split_it( split_it const &other ) noexcept
		  : m_data{ other.m_data }
		  , m_position{ other.m_position }
		  , m_splitter{ other.m_splitter } {}

		constexpr split_it( split_it &&other ) noexcept
		  : m_data{ DAW_MOVE( other.m_data ) }
		  , m_position{ DAW_MOVE( other.m_position ) }
		  , m_splitter{ DAW_MOVE( other.m_splitter ) } {}

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
				m_data = DAW_MOVE( rhs.m_data );
				m_position = DAW_MOVE( rhs.m_position );
				m_splitter = DAW_MOVE( rhs.m_splitter );
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
			split_it tmp{ *this };
			move_next( );
			return tmp;
		}

		constexpr split_it operator--( int ) const noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{ *this };
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
			split_it tmp{ *this };
			for( ; n > 0; --n ) {
				move_next( );
			}
			return tmp;
		}

		constexpr split_it operator-( ptrdiff_t n ) noexcept {
			static_assert( impl::can_decrement<Iterator>,
			               "Supplied Iterator is not Bidirectional" );
			split_it tmp{ *this };
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
			return split_it{ };
		}

		constexpr bool at_end( ) const noexcept {
			return m_position.begin( ) == m_data.cend( );
		}
	};

	namespace impl {
		template<typename String>
		using string_char_t = DAW_TYPEOF( *std::cbegin( std::declval<String>( ) ) );

		template<typename Splitter, typename String>
		constexpr bool is_splitter_v =
		  daw::traits::is_unary_predicate_v<Splitter, string_char_t<String>>;
	} // namespace impl

	template<typename String, typename Splitter,
	         daw::required<impl::is_splitter_v<Splitter, String>> = nullptr>
	constexpr auto make_split_it( String &sv, Splitter &&splitter ) noexcept {
		using IterT = DAW_TYPEOF( std::begin( sv ) );
		auto result = split_it<IterT, Splitter>{
		  std::begin( sv ), std::end( sv ), std::forward<Splitter>( splitter ) };
		return result;
	}

	template<typename CharT, typename Traits, typename Allocator>
	constexpr auto
	make_split_it( std::basic_string<CharT, Traits, Allocator> &str,
	               CharT divider ) noexcept {
		using SpltT = char_splitter_t<CharT>;
		using iterator = DAW_TYPEOF( str.begin( ) );

		return split_it<iterator, SpltT>( str.begin( ), str.end( ),
		                                  SpltT{ divider } );
	}
} // namespace daw

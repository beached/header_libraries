// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <array>
#include <exception>
#include <iterator>
#include <string>
#include <type_traits>

namespace daw::string_concat_impl {
	template<std::size_t StringCount>
	struct string_join_iterator {
		static_assert( StringCount > 0 );
		using CharT = char;
		using value_type = CharT;
		using pointer = value_type const *;
		using reference = value_type const &;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::random_access_iterator_tag;

	private:
		std::array<std::string_view, StringCount> m_strings{ };
		pointer m_first = nullptr;
		std::size_t m_idx = StringCount;

		constexpr bool empty( ) const noexcept {
			return m_idx >= StringCount;
		}

	public:
		constexpr string_join_iterator( ) = default;

		template<typename... Strings,
		         std::enable_if_t<( std::is_same_v<std::string, Strings> and ... ),
		                          std::nullptr_t> = nullptr>
		constexpr string_join_iterator( Strings const &...strings )
		  : m_strings{ std::string_view( strings )... }
		  , m_first{ std::data( m_strings[0] ) }
		  , m_idx{ 0 } {}

		constexpr value_type operator*( ) const noexcept {
			return *m_first;
		}

		constexpr pointer operator->( ) const noexcept {
			return m_first;
		}

		constexpr string_join_iterator &operator++( ) noexcept {
			++m_first;
			if( m_first != m_strings[m_idx].end( ) ) {
				return *this;
			}
			++m_idx;
			if( empty( ) ) {
				m_first = std::string_view{ nullptr, 0 }.begin( );
				return *this;
			}
			auto const &item = m_strings[m_idx];
			m_first = item.begin( );
			return *this;
		}

		constexpr string_join_iterator operator++( int ) noexcept {
			auto result = *this;
			this->operator++( );
			return result;
		}

		[[noreturn]] string_join_iterator &operator--( ) noexcept {
			std::terminate( );
		}

		[[noreturn]] string_join_iterator operator--( int ) noexcept {
			std::terminate( );
		}

		[[noreturn]] string_join_iterator &operator+=( difference_type ) const {
			std::terminate( );
		}

		[[noreturn]] string_join_iterator &operator-=( difference_type ) const {
			std::terminate( );
		}

		[[noreturn]] string_join_iterator operator+( difference_type ) const {
			std::terminate( );
		}

		[[noreturn]] string_join_iterator operator-( difference_type ) const {
			std::terminate( );
		}

		[[noreturn]] value_type operator[]( std::size_t ) const {
			std::terminate( );
		}

		[[noreturn]] bool operator<( string_join_iterator const & ) const {
			std::terminate( );
		}

		[[noreturn]] bool operator>( string_join_iterator const & ) const {
			std::terminate( );
		}

		[[noreturn]] bool operator<=( string_join_iterator const & ) const {
			std::terminate( );
		}

		[[noreturn]] bool operator>=( string_join_iterator const & ) const {
			std::terminate( );
		}

		constexpr difference_type
		operator-( string_join_iterator const &rhs ) const {
			if( not empty( ) ) {
				std::terminate( );
			}
			std::size_t result = 0;
			for( auto const sv : rhs.m_strings ) {
				result += sv.size( );
			}
			return result;
		}

		constexpr bool operator==( string_join_iterator const &rhs ) noexcept {
			return m_first == rhs.m_first;
		}

		constexpr bool operator!=( string_join_iterator const &rhs ) noexcept {
			return m_first != rhs.m_first;
		}
	};
	template<typename... Strings>
	string_join_iterator( Strings... )
	  -> string_join_iterator<sizeof...( Strings )>;
} // namespace daw::string_concat_impl

namespace daw {
	template<typename... Strings>
	inline std::string string_concat( std::string arg, Strings &&...strings ) {
		auto const appender = []( std::string &lhs, auto &&rhs ) {
			lhs.append( DAW_FWD( rhs ) );
		};
		arg.reserve( ( std::size( strings ) + ... ) );
		(void)( ( appender( arg, DAW_FWD( strings ) ), 0 ) + ... );
		return arg;
	}
} // namespace daw

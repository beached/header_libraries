// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_algorithm.h"
#include "daw_exchange.h"
#include "daw_newhelper.h"
#include "daw_operators.h"
#include "daw_scope_guard.h"
#include "daw_swap.h"
#include "daw_traits.h"

#include <cstdint>
#include <string>

namespace daw {
	template<typename CharT = char>
	class [[deprecated]] CString {
		CharT const *m_data = nullptr;
		size_t m_size = 0;
		bool m_local_string = true;

		[[deprecated]] static constexpr size_t
		string_length( CharT const *ptr ) noexcept {
			size_t result = 0;
			while( ptr[result] != 0 ) {
				++result;
			}
			return result;
		}

		struct no_copy_t {};

		[[deprecated]] constexpr CString( CharT const *ptr, no_copy_t,
		                                  size_t const length ) noexcept
		  : m_data{ ptr }
		  , m_size{ 0 == length ? string_length( ptr ) : length }
		  , m_local_string{ false } {}

	public:
		[[deprecated]] constexpr CString( CharT const *ptr, bool do_copy,
		                                  size_t const length )
		  : m_data{ ptr }
		  , m_size{ 0 == length ? string_length( ptr ) : length }
		  , m_local_string{ do_copy } {

			if( do_copy ) {
				size_t len = length;
				auto tmp = std::make_unique<CharT[]>( len + 1 );
				daw::algorithm::copy_n( ptr, tmp, len + 1 );
				m_data = tmp.release( );
			}
		}

		[[deprecated]] constexpr CString( CharT const *ptr, bool do_copy )
		  : CString{ ptr, do_copy, 0 } {}

		[[deprecated]] constexpr CString( CharT const *ptr )
		  : CString{ ptr, no_copy_t{ }, 0 } {}

		[[deprecated]] CString( ) = default;

		[[deprecated]] CString( CString const &value )
		  : CString{ value.m_data, true, value.m_size } {}

		[[deprecated]] CString &operator=( CString const &rhs ) {
			if( this != &rhs ) {
				CString tmp{ rhs };
				tmp.swap( *this );
			}
			return *this;
		}

		[[deprecated]] CString &operator=( CharT const *ptr ) {
			CString tmp{ ptr, true };
			tmp.swap( *this );
			return *this;
		}

		[[deprecated]] CString( CString && ) = default;
		[[deprecated]] CString &operator=( CString && ) = default;

		[[deprecated]] void swap( CString &rhs ) noexcept {
			daw::cswap( m_data, rhs.m_data );
			daw::cswap( m_size, rhs.m_size );
			daw::cswap( m_local_string, rhs.m_local_string );
		}

		[[deprecated]] ~CString( ) noexcept {
			auto tmp = daw::exchange( m_data, nullptr );
			if( m_local_string and nullptr != tmp ) {
				m_local_string = false;
				m_size = 0;
				delete[] tmp;
			}
			nullify( );
		}

		[[deprecated]] constexpr CharT const &
		operator[]( size_t pos ) const noexcept {
			return m_data[pos];
		}

		[[deprecated]] constexpr void nullify( ) noexcept {
			m_data = nullptr;
			m_size = 0;
			m_local_string = false;
		}

		[[deprecated]] constexpr CharT *move( ) noexcept {
			auto result = m_data;
			nullify( );
			return result;
		}

		[[deprecated]] constexpr CharT const *get( ) noexcept {
			return m_data;
		}

		[[deprecated]] std::string to_string( ) const {
			return std::string{ m_data, m_size };
		}

		[[deprecated]] constexpr size_t size( ) const noexcept {
			return m_size;
		}

		[[deprecated]] constexpr bool is_null( ) const noexcept {
			return nullptr == m_data;
		}

		[[deprecated]] constexpr bool empty( ) const noexcept {
			return nullptr == m_data or 0 == m_size;
		}

		[[deprecated]] explicit constexpr operator bool( ) const noexcept {
			return !empty( );
		}

		[[deprecated]] constexpr bool is_local_string( ) const noexcept {
			return m_local_string;
		}

		[[deprecated]] constexpr void take_ownership_of_data( ) noexcept {
			m_local_string = true;
		}

		[[deprecated]] auto compare( CString const &rhs ) const noexcept {
			return strcmp( m_data, rhs.m_data );
		}

		create_friend_comparison_operators( CString )
	}; // CString

	template<typename... Args>
	[[deprecated]] void swap( CString<Args...> &lhs,
	                          CString<Args...> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename Char>
	[[deprecated]] std::string to_string( CString<Char> const &str ) {
		return str.to_string( );
	}

	using cstring = CString<char>;
} // namespace daw

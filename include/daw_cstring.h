// The MIT License (MIT)
//
// Copyright (c) 2013-2017 Darrell Wright
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

#include <cstdint>
#include <string>

#include "daw_newhelper.h"
#include "daw_operators.h"
#include "daw_traits.h"
#include "scope_guard.h"

namespace daw {
	template<typename CharT = char>
	class CString {
		CharT const *m_data;
		bool m_local_string;

	public:
		CString( CharT const *ptr, bool do_copy = false, const size_t length = 0 ) : m_data{ptr}, m_local_string{do_copy} {
			if( do_copy ) {
				size_t len = length;
				if( 0 == len ) {
					len = strlen( ptr );
				}
				CharT *tmp = nullptr;
				SCOPE_EXIT {
					m_data = tmp;
				};
				tmp = new_array_throw<CharT>( len + 1 );
				memcpy( tmp, ptr, len + 1 );
			}
		}

		constexpr CString( ) noexcept : m_data{nullptr}, m_local_string{true} {}

		CString( CString const &value ) : CString{value.m_data, true} {}

		CString &operator=( CString const &rhs ) {
			if( this != &rhs ) {
				CString tmp{rhs};
				tmp.swap( *this );
			}
			return *this;
		}

		CString &operator=( CharT const *ptr ) {
			CString tmp{ptr, true};
			this->swap( tmp );
			return *this;
		}

		CString( CString &&value ) noexcept = default;
		CString &operator=( CString &&rhs ) noexcept = default;

		void swap( CString &rhs ) noexcept {
			using std::swap;
			swap( m_data, rhs.m_data );
			swap( m_local_string, rhs.m_local_string );
		}

		~CString( ) noexcept {
			if( m_local_string && nullptr != m_data ) {
				auto tmp = m_data;
				delete[] tmp;
			}
			nullify( );
		}

		constexpr CharT const &operator[]( size_t pos ) const {
			return m_data[pos];
		}

		constexpr void nullify( ) noexcept {
			m_data = nullptr;
			m_local_string = false;
		}

		constexpr CharT *move( ) noexcept {
			auto result = m_data;
			nullify( );
			return result;
		}

		constexpr CharT const *get( ) noexcept {
			return m_data;
		}

		std::string to_string( ) const {
			return std::string( m_data );
		}

		size_t size( ) const noexcept {
			if( is_null( ) ) {
				return 0;
			}
			return strlen( m_data );
		}

		constexpr bool is_null( ) const noexcept {
			return nullptr == m_data;
		}

		bool empty( ) const noexcept {
			return nullptr == m_data || strlen( m_data ) == 0;
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		constexpr bool is_local_string( ) const noexcept {
			return m_local_string;
		}

		constexpr void take_ownership_of_data( ) noexcept {
			m_local_string = true;
		}

		auto compare( CString const &rhs ) const {
			return strcmp( m_data, rhs.m_data );
		}

		create_friend_comparison_operators( CString )
	}; // CString

	template<typename... Args>
	constexpr void swap( CString<Args...> &lhs, CString<Args...> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename Char>
	std::string to_string( CString<Char> const &str ) {
		return str.to_string( );
	}

	using cstring = CString<char>;
} // namespace daw

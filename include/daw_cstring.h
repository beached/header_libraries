// The MIT License (MIT)
//
// Copyright (c) 2013-2018 Darrell Wright
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

#include "daw_algorithm.h"
#include "daw_newhelper.h"
#include "daw_operators.h"
#include "daw_scope_guard.h"
#include "daw_traits.h"

namespace daw {
	template<typename CharT = char>
	class CString {
		CharT const *m_data;
		size_t m_size;
		bool m_local_string;

		static constexpr size_t string_length( CharT const *ptr ) noexcept {
			size_t result = 0;
			while( ptr[result] != 0 ) {
				++result;
			}
			return result;
		}

		struct no_copy_t {};
		constexpr CString( CharT const *ptr, no_copy_t, size_t const length ) noexcept
		  : m_data{ptr}
		  , m_size{0 == length ? string_length( ptr ) : length}
		  , m_local_string{false} {}

	public:
		CString( CharT const *ptr, bool do_copy, size_t const length )
		  : m_data{ptr}
		  , m_size{0 == length ? string_length( ptr ) : length}
		  , m_local_string{do_copy} {

			if( do_copy ) {
				size_t len = length;
				auto tmp = std::make_unique<CharT[]>( len + 1 );
				daw::algorithm::copy_n( ptr, tmp, len + 1 );
				m_data = tmp.release( );
			}
		}

		CString( CharT const *ptr, bool do_copy )
		  : CString{ptr, do_copy, 0} {}

		constexpr CString( CharT const *ptr )
		  : CString{ptr, no_copy_t{}, 0} {}

		constexpr CString( ) noexcept
		  : m_data{nullptr}
		  , m_size{0}
		  , m_local_string{true} {}

		CString( CString const &value )
		  : CString{value.m_data, true, value.m_size} {}

		CString &operator=( CString const &rhs ) {
			if( this != &rhs ) {
				CString tmp{rhs};
				tmp.swap( *this );
			}
			return *this;
		}

		CString &operator=( CharT const *ptr ) {
			CString tmp{ptr, true};
			tmp.swap( *this );
			return *this;
		}

		constexpr CString( CString &&value ) noexcept = default;
		constexpr CString &operator=( CString &&rhs ) noexcept = default;

		void swap( CString &rhs ) noexcept {
			using std::swap;
			swap( m_data, rhs.m_data );
			swap( m_size, rhs.m_size );
			swap( m_local_string, rhs.m_local_string );
		}

		~CString( ) noexcept {
			auto tmp = std::exchange( m_data, nullptr );
			if( m_local_string && nullptr != tmp ) {
				m_local_string = false;
				m_size = 0;
				delete[] tmp;
			}
			nullify( );
		}

		constexpr CharT const &operator[]( size_t pos ) const noexcept {
			return m_data[pos];
		}

		constexpr void nullify( ) noexcept {
			m_data = nullptr;
			m_size = 0;
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
			return std::string{m_data, m_size};
		}

		constexpr size_t size( ) const noexcept {
			return m_size;
		}

		constexpr bool is_null( ) const noexcept {
			return nullptr == m_data;
		}

		constexpr bool empty( ) const noexcept {
			return nullptr == m_data || 0 == m_size;
		}

		explicit constexpr operator bool( ) const noexcept {
			return !empty( );
		}

		constexpr bool is_local_string( ) const noexcept {
			return m_local_string;
		}

		constexpr void take_ownership_of_data( ) noexcept {
			m_local_string = true;
		}

		auto compare( CString const &rhs ) const noexcept {
			return strcmp( m_data, rhs.m_data );
		}

		create_friend_comparison_operators( CString )
	}; // CString

	template<typename... Args>
	void swap( CString<Args...> &lhs, CString<Args...> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename Char>
	std::string to_string( CString<Char> const &str ) {
		return str.to_string( );
	}

	using cstring = CString<char>;
} // namespace daw

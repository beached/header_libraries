// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_move.h"

#include <string>

namespace daw {
	template<typename SanitizeFunction, typename charT,
	         typename traits = std::char_traits<charT>,
	         typename Alloc = std::allocator<charT>>
	struct basic_safe_string {
		using string_type = std::basic_string<charT, traits, Alloc>;

	private:
		string_type m_unsafe_string;
		SanitizeFunction m_sanitize_function;

	public:
		basic_safe_string( ) = default;
		~basic_safe_string( ) = default;
		basic_safe_string( basic_safe_string const & ) = default;
		basic_safe_string( basic_safe_string && ) = default;
		basic_safe_string &operator=( basic_safe_string const & ) = default;
		basic_safe_string &operator=( basic_safe_string && ) = default;

		basic_safe_string(
		  string_type unsafe_string,
		  SanitizeFunction sanitize_function = SanitizeFunction{ } )
		  : m_unsafe_string{ DAW_MOVE( unsafe_string ) }
		  , m_sanitize_function{ DAW_MOVE( sanitize_function ) } {}

		basic_safe_string(
		  charT const *const unsafe_cstring,
		  SanitizeFunction sanitize_function = SanitizeFunction{ } )
		  : basic_safe_string{ string_type{ unsafe_cstring },
		                       DAW_MOVE( sanitize_function ) } {}

		basic_safe_string &operator=( string_type unsafe_string ) {
			m_unsafe_string = DAW_MOVE( unsafe_string );
			return *this;
		}

		string_type get( ) const {
			return m_sanitize_function( m_unsafe_string );
		}

		string_type const &unsafe_get( ) {
			return m_unsafe_string;
		}

		explicit operator string_type( ) const {
			return get( );
		}
	}; // basic_safe_string

	template<typename SanitizeFunction>
	using safe_string = basic_safe_string<SanitizeFunction, char>;

	template<typename SanitizeFunction>
	using safe_wstring = basic_safe_string<SanitizeFunction, wchar_t>;

	template<typename SanitizeFunction>
	using safe_u16string = basic_safe_string<SanitizeFunction, char16_t>;

	template<typename SanitizeFunction>
	using safe_u32string = basic_safe_string<SanitizeFunction, char32_t>;

	template<typename SanitizeFunction, typename charT, typename traits,
	         typename Alloc>
	auto make_safe_string( std::basic_string<charT, traits, Alloc> unsafe_string,
	                       SanitizeFunction &&sanitize_function ) {
		return basic_safe_string<SanitizeFunction, charT, traits, Alloc>{
		  DAW_MOVE( unsafe_string ),
		  std::forward<SanitizeFunction>( sanitize_function ) };
	}

	template<typename SanitizeFunction, typename charT,
	         typename traits = std::char_traits<charT>,
	         typename Alloc = std::allocator<charT>>
	auto make_safe_string( charT const *const unsafe_cstring,
	                       SanitizeFunction &&sanitize_function ) {
		return basic_safe_string<SanitizeFunction, charT, traits, Alloc>{
		  unsafe_cstring, std::forward<SanitizeFunction>( sanitize_function ) };
	}
} // namespace daw

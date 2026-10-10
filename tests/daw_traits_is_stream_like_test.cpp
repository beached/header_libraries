// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/traits/daw_traits_is_istream_like.h>
#include <daw/traits/daw_traits_is_ostream_like.h>

#include <sstream>

namespace {
	struct ios_like {
		using char_type = char;
		static constexpr int adjustfield = 0;

		[[maybe_unused]] char fill( ) const;
		[[maybe_unused]] bool good( ) const;
		[[maybe_unused]] int width( ) const;
		[[maybe_unused]] int flags( ) const;
	};

	struct istream_like : ios_like {
		[[maybe_unused]] istream_like &read( char *, int );
	};

	struct ostream_like : ios_like {
		[[maybe_unused]] ostream_like &write( char const *, int );
	};

	struct non_const_ios_like {
		using char_type = char;
		[[maybe_unused]] static constexpr int adjustfield = 0;

		[[maybe_unused]] char fill( );
		[[maybe_unused]] bool good( );
		[[maybe_unused]] int width( );
		[[maybe_unused]] int flags( );
	};
} // namespace

// Standard narrow streams
static_assert( daw::traits::is_istream_like_v<std::istringstream, char> );
static_assert( not daw::traits::is_ostream_like_v<std::istringstream, char> );
static_assert( daw::traits::is_ostream_like_v<std::ostringstream, char> );
static_assert( not daw::traits::is_istream_like_v<std::ostringstream, char> );
static_assert( daw::traits::is_istream_like_v<std::stringstream, char> );
static_assert( daw::traits::is_ostream_like_v<std::stringstream, char> );

// Standard wide streams and mismatched character types
static_assert( daw::traits::is_istream_like_v<std::wistringstream, wchar_t> );
static_assert( daw::traits::is_ostream_like_v<std::wostringstream, wchar_t> );
static_assert( not daw::traits::is_istream_like_v<std::wistringstream, char> );
static_assert( not daw::traits::is_ostream_like_v<std::wostringstream, char> );

// Structurally stream-like types
static_assert( daw::traits::is_istream_like_lite_v<istream_like> );
static_assert( daw::traits::has_read_member_v<istream_like, char> );
static_assert( daw::traits::is_istream_like_v<istream_like, char> );
static_assert( not daw::traits::is_ostream_like_v<istream_like, char> );

static_assert( daw::traits::is_ostream_like_lite_v<ostream_like> );
static_assert( daw::traits::has_write_member_v<ostream_like, char> );
static_assert( daw::traits::is_ostream_like_v<ostream_like, char> );
static_assert( not daw::traits::is_istream_like_v<ostream_like, char> );

// Missing or incompatible operations
static_assert( not daw::traits::is_istream_like_v<ios_like, char> );
static_assert( not daw::traits::is_ostream_like_v<ios_like, char> );
static_assert( not daw::traits::is_istream_like_v<istream_like, wchar_t> );
static_assert( not daw::traits::is_ostream_like_v<ostream_like, wchar_t> );
static_assert( not daw::traits::is_istream_like_lite_v<non_const_ios_like> );
static_assert( not daw::traits::is_ostream_like_lite_v<non_const_ios_like> );
static_assert( not daw::traits::is_istream_like_v<int, char> );
static_assert( not daw::traits::is_ostream_like_v<int, char> );

int main( ) {}

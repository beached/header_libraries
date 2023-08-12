// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	/// @brief Tag type for specifying that the searched for term/item is not
	/// to be removed from view
	struct nodiscard_t {};
	inline constexpr auto nodiscard = nodiscard_t{ };

	/// @brief Tag type for specifying that the searched for term/item is
	/// to be removed from view
	struct discard_t {};
	inline constexpr auto discard = discard_t{ };

	struct dont_clip_to_bounds_t {};
	inline constexpr auto dont_clip_to_bounds = dont_clip_to_bounds_t{ };

	/// @brief A predicate type used in the find based routine to return true
	/// when the element is one of the specified characters
	template<typename T, T... needles>
	struct any_of_t {
		[[nodiscard]] inline constexpr bool operator( )( T c ) const {
			return ( ( c == needles ) | ... );
		}
	};

	/// @brief A predicate value used in the find based routine to return true
	/// when the element is one of the specified characters
	template<auto needle, auto... needles>
	inline static constexpr any_of_t<decltype( needle ), needle, needles...>
	  any_of{ };

	template<typename... Ts>
	[[nodiscard]] constexpr auto any_of_f( Ts &&...needles ) noexcept {
		static_assert( sizeof...( Ts ) > 0 );
		return [needles...]( auto const &haystack ) {
			return ( ( haystack == needles ) or ... );
		};
	}

	/// @brief Predicate type that stores the previous element and if it was an
	/// '\' escape character, it will return false, otherwise returns true if
	/// the current character matches one of the needles.  Because this is a
	/// stateful predicate, it cannot be reused
	template<auto needle, auto... needles>
	struct escaped_any_of {
		using CharT = decltype( needle );
		static_assert( not( needle == CharT{ '\\' } or
		                    ( ( needles == CharT{ '\\' } ) or ... ) ),
		               "Escape characters cannot be searched for" );
		CharT last_char = 0;
		[[nodiscard]] inline constexpr bool operator( )( CharT c ) {
			if( last_char == CharT( '\\' ) ) {
				last_char = 0;
				return false;
			}
			last_char = c;
			return needle == c or ( ( c == needles ) or ... );
		}
	};

	/// @brief A predicate type used in the find based routine to return true
	/// when the element is none of the specified characters
	template<typename CharT, CharT... needles>
	struct none_of_t {
		[[nodiscard]] inline constexpr bool operator( )( CharT c ) const {
			return ( ( c != needles ) & ... );
		}
	};

	template<auto needle, auto... needles>
	inline static constexpr none_of_t<decltype( needle ), needle, needles...>
	  none_of{ };

	template<typename... Ts>
	[[nodiscard]] constexpr auto none_of_f( Ts &&...needles ) noexcept {
		static_assert( sizeof...( Ts ) > 0 );
		return [needles...]( auto const &haystack ) {
			return ( ( haystack != needles ) and ... );
		};
	}
} // namespace daw

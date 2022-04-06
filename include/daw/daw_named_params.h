// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_concepts.h>
#include <daw/daw_move.h>
#include <daw/daw_traits.h>

#include <compare>
#include <type_traits>

namespace daw::nparam::nparam_details {
	/// @brief Satisfied when the type has the opt in type alias and has a value
	/// member
	template<typename T>
	concept OptionHelper = requires {
		                       typename std::remove_cvref_t<T>::i_am_an_option;
		                       std::declval<T>( ).value;
	                       };

	template<typename T, typename... Ts>
	inline constexpr bool
	  less_than_one_match_v = ( ( std::is_same_v<T, Ts> ? 1 : 0 ) + ... ) <= 1;

	template<typename... Opts>
	inline constexpr bool is_unique_v = ( less_than_one_match_v<Opts, Opts...> and
	                                      ... );
} // namespace daw::nparam::nparam_details

namespace daw::nparam {
	/// @brief Satisfied when all Opts satisfy the Option concept.
	/// a) Has i_am_an_option type alias
	/// b) has value member
	/// c) requires that each specified option is not repeated.
	/// @tparam Ts Options to test
	template<typename... Ts>
	concept Options = ( nparam_details::OptionHelper<Ts> and ... ) and
	                  nparam_details::is_unique_v<Ts...>;

	/// @brief Helper method to extract option from pack or use default value
	/// @tparam Option default option type and result to use if Option type not
	/// found in Opts
	/// @tparam Opts a pack of Options that the user has set
	/// @param def_value value
	/// @param opts user set options
	/// @return The Option set in opts or def_value
	template<Options Option, typename... Opts>
	  requires( Options<Opts...> )
	constexpr auto get_opt( Option &&def_value, Opts &&...vals ) {
		if constexpr( sizeof...( Opts ) == 0 ) {
			// No replacements found
			return DAW_FWD2( Option, def_value ).value;
		} else if constexpr( std::is_same_v<std::remove_cvref_t<Option>,
		                                    std::remove_cvref_t<
		                                      daw::traits::first_type<Opts...>>> ) {
			// Found a type match, use it for return value
			constexpr auto first_val = []( auto &&v, auto &&... ) {
				return DAW_FWD( v );
			};
			return first_val( DAW_FWD2( Opts, vals )... ).value;
		} else {
			// Pop off first option and try next
			constexpr auto pop_front = []( auto &&def, auto &&, auto &&...vs ) {
				return get_opt<Option>( DAW_FWD( def ),
				                                      DAW_FWD( vs )... );
			};
			return pop_front( DAW_FWD2( Option, def_value ),
			                  DAW_FWD2( Opts, vals )... );
		}
	}
	namespace details {
		template<typename Type, typename...>
		struct unique_option_t {
			using i_am_an_option = void;
			Type value;

			[[nodiscard]] constexpr unique_option_t operator=( Type &&v ) const {
				return unique_option_t{ DAW_MOVE( v ) };
			}
			auto operator<=>( unique_option_t const & ) const = default;
		};

		template<typename Type, typename...>
		struct option_t {
			using i_am_an_option = void;
			Type value;

			[[nodiscard]] constexpr option_t operator=( Type const &v ) const {
				return option_t{ v };
			}

			[[nodiscard]] constexpr option_t operator=( Type &&v ) const {
				return option_t{ DAW_MOVE( v ) };
			}
			auto operator<=>( option_t const & ) const = default;
		};
	} // namespace details
} // namespace daw::nparam

/// @brief Create a named option value of a unique type with value set.  Must be
/// assigned to a variable
#define DAW_MAKE_NAMED_OPTION( Type, Name, DefaultValue )                      \
	[] {                                                                         \
		struct opt_##Name##_t;                                                     \
		return ::daw::nparam::details::option_t<Type, opt_##Name##_t>{             \
		  DefaultValue };                                                          \
	}( )

/// @brief Create a named option value of a unique type with value set. Must be
/// assigned to a variable
#define DAW_MAKE_NAMED_UNIQUE_OPTION( Type, Name, DefaultValue )               \
	[] {                                                                         \
		struct opt_##Name##_t;                                                     \
		return ::daw::nparam::details::unique_option_t<Type, opt_##Name##_t>{      \
		  DefaultValue };                                                          \
	}( )

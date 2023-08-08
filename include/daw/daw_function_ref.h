// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_consteval.h"
#include "daw_traits.h"

#include <type_traits>

namespace daw {
	template<typename>
	class function_ref;

	template<typename Result, typename... Args>
	class function_ref<Result( Args... )> {
		void const *m_data = nullptr;
		fp_t<Result( Args..., void const * )> m_fp = nullptr;

	public:
		function_ref( ) = default;

		DAW_CONSTEVAL function_ref( std::nullptr_t ) noexcept {}

		template<typename T,
		         std::enable_if_t<not std::is_same_v<function_ref, T> and
		                            std::is_invocable_r_v<Result, T, Args...> and
		                            not std::is_function_v<T>,
		                          std::nullptr_t> = nullptr>
		constexpr function_ref( T const &fn ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( fn ) ) )
		  , m_fp( []( Args... args, void const *d ) -> Result {
			  return ( *reinterpret_cast<T const *>( d ) )( DAW_FWD( args )... );
		  } ) {}

		template<typename T,
		         std::enable_if_t<not std::is_same_v<function_ref, T> and
		                            std::is_invocable_r_v<Result, T, Args...> and
		                            std::is_function_v<T>,
		                          std::nullptr_t> = nullptr>
		inline function_ref( T const &fn ) noexcept
		  : m_data( reinterpret_cast<void const *>( std::addressof( fn ) ) )
		  , m_fp( []( Args... args, void const *d ) -> Result {
			  return ( *reinterpret_cast<T const *>( d ) )( DAW_FWD( args )... );
		  } ) {}

		inline Result operator( )( Args... args ) const {
			return m_fp( DAW_FWD( args )..., m_data );
		}

		explicit constexpr operator bool( ) const noexcept {
			return m_fp != nullptr;
		}

		constexpr void reset( ) noexcept {
			m_data = nullptr;
			m_fp = nullptr;
		}

		constexpr void const *data( ) const noexcept {
			return m_data;
		}

		friend constexpr bool operator==( function_ref<Result( Args... )> fn,
		                                  std::nullptr_t ) noexcept {
			return static_cast<bool>( fn );
		}

		friend constexpr bool
		operator==( std::nullptr_t, function_ref<Result( Args... )> fn ) noexcept {
			return static_cast<bool>( fn );
		}

		friend constexpr bool operator!=( function_ref<Result( Args... )> fn,
		                                  std::nullptr_t ) noexcept {
			return not static_cast<bool>( fn );
		}

		friend constexpr bool
		operator!=( std::nullptr_t, function_ref<Result( Args... )> fn ) noexcept {
			return not static_cast<bool>( fn );
		}
	};
} // namespace daw

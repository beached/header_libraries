// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_consteval.h"
#include "daw_likely.h"
#include "daw_traits.h"

#include <cassert>
#include <type_traits>

namespace daw {
	template<typename>
	class function_ref;

	template<typename Result, typename... Args>
	class function_ref<Result( Args... )> {
		void const *m_data;
		fp_t<Result( Args..., void const * )> m_fp;

	public:
		function_ref( std::nullptr_t ) = delete;

		template<typename T,
		         std::enable_if_t<not std::is_same_v<function_ref, T> and
		                            std::is_invocable_v<T, Args...> and
		                            std::is_convertible_v<
		                              std::invoke_result_t<T, Args...>, Result>,
		                          std::nullptr_t> = nullptr>
		constexpr function_ref( T const &fn ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( fn ) ) )
		  , m_fp( []( Args... args, void const *d ) -> Result {
			  auto const &obj = *reinterpret_cast<T const *>( d );
			  return static_cast<Result>( obj( DAW_FWD( args )... ) );
		  } ) {}

		template<typename T,
		         std::enable_if_t<not std::is_same_v<function_ref, T> and
		                            std::is_invocable_v<T, Args...> and
		                            std::is_convertible_v<
		                              std::invoke_result_t<T, Args...>, Result>,
		                          std::nullptr_t> = nullptr>
		constexpr function_ref &operator=( T const &fn ) noexcept {
			m_data = static_cast<void const *>( std::addressof( fn ) );
			m_fp = []( Args... args, void const *d ) -> Result {
				auto const &obj = *reinterpret_cast<T const *>( d );
				return static_cast<Result>( obj( DAW_FWD( args )... ) );
			};
		}

		template<typename R, typename... As,
		         std::enable_if_t<std::is_invocable_v<R ( * )( As... ), Args...> and
		                            std::is_convertible_v<R, Result>,
		                          std::nullptr_t> = nullptr>
		inline function_ref( fp_t<R( As... )> fp ) noexcept
		  : m_data( reinterpret_cast<void const *>( fp ) )
		  , m_fp( []( Args... args, void const *d ) -> Result {
			  auto const f =
			    reinterpret_cast<fp_t<R( As... )>>( const_cast<void *>( d ) );
			  return static_cast<Result>( f( DAW_FWD( args )... ) );
		  } ) {
			assert( fp != nullptr );
		}

		template<typename R, typename... As,
		         std::enable_if_t<std::is_invocable_v<R ( * )( As... ), Args...> and
		                            std::is_convertible_v<R, Result>,
		                          std::nullptr_t> = nullptr>
		inline function_ref &operator=( fp_t<R( As... )> fp ) noexcept {
			assert( fp != nullptr );
			m_data = reinterpret_cast<void const *>( fp );
			m_fp = []( Args... args, void const *d ) -> Result {
				auto const f =
				  reinterpret_cast<fp_t<R( As... )>>( const_cast<void *>( d ) );
				return static_cast<Result>( f( DAW_FWD( args )... ) );
			};
		}

		template<typename R,
		         std::enable_if_t<not std::is_invocable_r_v<Result, R, Args...> and
		                            std::is_convertible_v<R, Result>,
		                          std::nullptr_t> = nullptr>
		constexpr function_ref( R const &r ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( r ) ) )
		  , m_fp( []( Args..., void const *d ) -> Result {
			  auto const &result = *reinterpret_cast<R const *>( d );
			  return static_cast<Result>( result );
		  } ) {}

		template<typename R,
		         std::enable_if_t<not std::is_invocable_r_v<Result, R, Args...> and
		                            std::is_convertible_v<R, Result>,
		                          std::nullptr_t> = nullptr>
		constexpr function_ref &operator=( R const &r ) noexcept {
			m_data = static_cast<void const *>( std::addressof( r ) );
			m_fp = []( Args..., void const *d ) -> Result {
				auto const &result = *reinterpret_cast<R const *>( d );
				return static_cast<Result>( result );
			};
		}

		constexpr Result operator( )( Args... args ) const {
			return m_fp( DAW_FWD( args )..., m_data );
		}

		constexpr void const *data( ) const noexcept {
			return m_data;
		}
	};
} // namespace daw

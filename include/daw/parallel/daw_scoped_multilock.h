// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <mutex>
#include <tuple>

namespace daw {
	namespace details {
		template<size_t N, size_t SZ>
		struct make_lock_guard {
			template<typename Results, typename TPLockables>
			void operator( )( Results &results, TPLockables &lockables ) const {
				auto current_lockable = std::get<N>( lockables );
				using current_lock_t =
				  std::lock_guard<std::decay_t<decltype( *current_lockable )>>;

				std::get<N>( results ) = std::make_unique<current_lock_t>(
				  *current_lockable, std::adopt_lock );
				make_lock_guard<N + 1, SZ>{ }( results, lockables );
			}
		};

		template<size_t N>
		struct make_lock_guard<N, N> {
			template<typename Results, typename TPLockables>
			constexpr void operator( )( Results const &,
			                            TPLockables const & ) const noexcept {}
		};
	} // namespace details

	template<typename... Lockables>
	class scoped_multilock {
		static_assert( sizeof...( Lockables ) > 0,
		               "Must specify at least 1 lockable" );
		std::tuple<std::unique_ptr<std::lock_guard<std::decay_t<Lockables>>>...>
		  m_locks;

		template<typename Arg>
		void _lock( Arg const & ) {}

		template<typename Arg1, typename Arg2, typename... Args>
		void _lock( Arg1 &arg1, Arg2 &arg2, Args &... args ) {
			std::lock( arg1, arg2, args... );
		}

	public:
		scoped_multilock( Lockables &... lockables )
		  : m_locks{ } {
			_lock( lockables... );
			auto args = std::make_tuple<Lockables *...>( &lockables... );
			details::make_lock_guard<0, sizeof...( Lockables )>{ }( m_locks, args );
		}

		~scoped_multilock( ) = default;
		scoped_multilock( scoped_multilock && ) noexcept = default;
		scoped_multilock &operator=( scoped_multilock && ) noexcept = default;

		scoped_multilock( scoped_multilock const & ) = delete;
		scoped_multilock &operator=( scoped_multilock const & ) = delete;
	};

	template<typename... Lockables>
	auto make_scoped_multilock( Lockables &... lockables ) {
		return scoped_multilock<Lockables...>{ lockables... };
	}
} // namespace daw

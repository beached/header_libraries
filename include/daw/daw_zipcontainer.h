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
#include "daw_traits.h"
#include "iterator/daw_zipiter.h"

namespace daw {
	template<typename... Containers>
	struct zip_container {
		using iterator = DAW_TYPEOF(
		  zip_iterator( std::begin( std::declval<Containers &>( ) )... ) );
		using const_iterator = DAW_TYPEOF(
		  zip_iterator( std::begin( std::declval<Containers const &>( ) )... ) );

	private:
		iterator m_begin;
		iterator m_end;

	public:
		constexpr zip_container( Containers... containers )
		  : m_begin( std::begin( containers )... )
		  , m_end( std::end( containers )... ) {}

		constexpr iterator begin( ) {
			return m_begin;
		}

		constexpr const_iterator begin( ) const {
			return m_begin;
		}

		constexpr const_iterator cbegin( ) const {
			return m_begin;
		}

		constexpr iterator end( ) {
			return m_end;
		}

		constexpr const_iterator end( ) const {
			return m_end;
		}

		constexpr const_iterator cend( ) const {
			return m_end;
		}
	}; // struct zip_container

	template<typename... Containers>
	zip_container( Containers &&... ) -> zip_container<Containers...>;

	template<typename... Containers>
	zip_container<Containers...> make_zipcontainer( Containers &&...args ) {
		return zip_container<Containers...>( DAW_FWD( args )... );
	}
} // namespace daw

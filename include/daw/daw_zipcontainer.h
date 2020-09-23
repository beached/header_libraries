// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "iterator/daw_zipiter.h"

#include <ciso646>

namespace daw {
	template<typename... Containers>
	struct zip_container {
		using iterator =
		  zip_iterator<decltype( std::declval<Containers...>( ).begin( ) )>;
		using const_iterator = zip_iterator<decltype(
		  std::declval<std::add_const_t<Containers>>( ).begin( ) )...>;

	private:
		iterator m_begin;
		iterator m_end;

	public:
		zip_container( Containers... containers )
		  : m_begin( make_zipiter( std::begin( containers )... ) )
		  , m_end( make_zipiter( std::end( containers )... ) ) {}

		iterator begin( ) {
			return m_begin;
		}

		const_iterator begin( ) const {
			return m_begin;
		}

		const_iterator cbegin( ) const {
			return m_begin;
		}

		iterator end( ) {
			return m_end;
		}

		const_iterator end( ) const {
			return m_end;
		}

		const_iterator cend( ) const {
			return m_end;
		}
	}; // struct zip_container

	template<typename... Containers>
	zip_container<Containers...> make_zipcontainer( Containers... args ) {
		return zip_container<Containers...>( args... );
	}
} // namespace daw

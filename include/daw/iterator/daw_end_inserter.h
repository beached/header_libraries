// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_17.h"
#include "daw/daw_enable_if.h"
#include "daw_function_iterator.h"

#include <type_traits>
#include <utility>

namespace daw {
	template<typename Container>
	struct end_inserter_iterator final {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Container *m_container;

	public:
		explicit constexpr end_inserter_iterator( Container &c ) noexcept
		  : m_container( &c ) {}

		template<typename T,
		         daw::enable_when_t<
		           !std::is_same_v<daw::remove_cvref_t<T>, end_inserter_iterator>> =
		           nullptr>
		constexpr end_inserter_iterator &operator=( T &&val ) {
			m_container->insert( std::end( *m_container ), DAW_FWD( val ) );
			return *this;
		}

		[[nodiscard]] constexpr end_inserter_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr end_inserter_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr end_inserter_iterator &operator++( int ) noexcept {
			return *this;
		}
	};
	template<typename Container>
	end_inserter_iterator( Container ) -> end_inserter_iterator<Container>;

	template<typename Container>
	[[nodiscard]] constexpr auto end_inserter( Container &c ) {
		return end_inserter_iterator<Container>( c );
	}
} // namespace daw

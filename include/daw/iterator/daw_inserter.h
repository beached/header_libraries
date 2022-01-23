// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../cpp_17.h"
#include "../daw_enable_if.h"
#include "../daw_move.h"
#include "daw_function_iterator.h"

#include <ciso646>
#include <iterator>
#include <type_traits>

namespace daw {
	template<typename Container>
	struct inserter_iterator final {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Container *m_container;

	public:
		constexpr inserter_iterator( Container &c ) noexcept
		  : m_container( &c ) {}

		template<typename T,
		         daw::enable_when_t<!std::is_same_v<daw::remove_cvref_t<T>,
		                                            inserter_iterator>> = nullptr>
		constexpr inserter_iterator &operator=( T &&val ) {
			m_container->insert( DAW_FWD2( T, val ) );
			return *this;
		}

		constexpr inserter_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr inserter_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr inserter_iterator &operator++( int ) noexcept {
			return *this;
		}
	};

	template<typename Container>
	constexpr auto inserter( Container &c ) {
		return inserter_iterator<Container>( c );
	}
} // namespace daw

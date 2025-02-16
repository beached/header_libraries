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
#include "daw/daw_move.h"
#include "daw_function_iterator.h"

#include <iterator>
#include <type_traits>

namespace daw {
	template<typename Container>
	struct back_inserter final {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Container *m_container;

	public:
		explicit constexpr back_inserter( Container &c ) noexcept
		  : m_container( &c ) {}

		template<typename T,
		         daw::enable_when_t<not std::is_same_v<daw::remove_cvref_t<T>,
		                                               back_inserter>> = nullptr>
		constexpr back_inserter &operator=( T &&val ) {
			m_container->push_back( DAW_FWD( val ) );
			return *this;
		}

		constexpr back_inserter &operator*( ) noexcept {
			return *this;
		}

		constexpr back_inserter &operator++( ) noexcept {
			return *this;
		}

		constexpr back_inserter operator++( int ) const noexcept {
			return *this;
		}
	};

	template<typename Container>
	back_inserter( Container ) -> back_inserter<Container>;
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/pipelines/range_base.h"

#include <memory>

namespace daw::pipelines {
	template<typename T>
	struct maybe_view : range_base_t<std::remove_reference_t<T> *> {
		using value_type = std::remove_reference_t<T>;
		using reference = T &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

		pointer m_value = nullptr;

		explicit maybe_view( ) = default;

		explicit constexpr maybe_view( reference value )
		  : m_value( std::addressof( value ) ) {}

		// TODO Use iterator with ref return
		[[nodiscard]] constexpr pointer begin( ) {
			return m_value;
		}

		[[nodiscard]] constexpr const_pointer begin( ) const {
			return m_value;
		}

		[[nodiscard]] constexpr pointer end( ) {
			if( m_value ) {
				return m_value + 1;
			}
			return nullptr;
		}

		[[nodiscard]] constexpr const_pointer end( ) const {
			if( m_value ) {
				return m_value + 1;
			}
			return nullptr;
		}
	};
	template<typename T>
	maybe_view( T && ) -> maybe_view<T>;
} // namespace daw::pipelines

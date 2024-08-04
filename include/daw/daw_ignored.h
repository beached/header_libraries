// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"

namespace daw {
	namespace {
		struct ignored_t {
			ignored_t( ) = default;

			template<typename T>
			DAW_ATTRIB_INLINE constexpr ignored_t( T && ) noexcept {}

			template<typename T>
			DAW_ATTRIB_INLINE constexpr ignored_t const &operator=( T && ) const noexcept {
				return *this;
			}

			template<typename T>
			DAW_ATTRIB_INLINE constexpr ignored_t &operator=( T && ) noexcept {
				return *this;
			}
		};

		inline constexpr ignored_t ignored = ignored_t{ };

		template<auto...>
		using ignore_constants_t = ignored_t;

		template<typename...>
		using ignore_types_t = ignored_t;	
	} // namespace
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_traits.h"

#include <atomic>

namespace daw {
	template<typename T>
	class countable_resource_t {
		static std::atomic<T> m_resource_count;

	public:
		countable_resource_t( ) noexcept {
			++m_resource_count;
		}

		countable_resource_t( countable_resource_t const & ) noexcept {
			++m_resource_count;
		}

		countable_resource_t &operator=( countable_resource_t const & ) noexcept {
			++m_resource_count;
		}

		~countable_resource_t( ) {
			--m_resource_count;
		}

		[[nodiscard]] static T value( ) noexcept {
			return m_resource_count.load( );
		}
	};

	template<typename T>
	std::atomic<T> countable_resource_t<T>::m_resource_count = { };
} // namespace daw

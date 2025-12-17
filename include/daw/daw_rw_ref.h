// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_not_null.h"

#include <memory>

namespace daw {
	// A const propegating reference wrapper
	template<typename T>
	struct rw_ref {
		static_assert( not std::is_reference_v<T>, "T cannot be a reference type" );
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using pointer = T *;

	private:
		daw::not_null<pointer> m_ptr;

	public:
		DAW_ATTRIB_INLINE constexpr rw_ref( reference ref ) noexcept
		  : m_ptr( never_null, std::addressof( ref ) ) {}

		DAW_ATTRIB_INLINE constexpr rw_ref &operator=( reference ref ) noexcept {
			m_ptr = std::addressof( ref );
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr reference get( ) noexcept {
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr const_reference get( ) const noexcept {
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr operator reference( ) noexcept {
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr operator const_reference( ) const noexcept {
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr reference operator( )( ) noexcept {
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr const_reference operator( )( ) const noexcept {
			return *m_ptr;
		}
	};

	template<typename T>
	rw_ref( T & ) -> rw_ref<T>;
} // namespace daw

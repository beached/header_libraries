// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_assume.h"
#include "daw_attributes.h"

#include <ciso646>
#include <iterator>
#include <type_traits>

namespace daw {
	template<typename Pointer>
	struct not_null {
		static_assert( std::is_pointer_v<Pointer>, "Only pointers are supported" );

		using pointer = Pointer;
		using reference = typename std::iterator_traits<pointer>::reference;

	private:
		pointer m_ptr;

	public:
		DAW_ATTRIB_INLINE constexpr not_null( pointer p ) noexcept: m_ptr( p ) {
			if( not p ) {
				std::terminate( );
			}
		}

		DAW_ATTRIB_INLINE constexpr reference operator*( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return *m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr pointer operator->( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr pointer get( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		DAW_ATTRIB_INLINE constexpr operator pointer( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}
	};
}



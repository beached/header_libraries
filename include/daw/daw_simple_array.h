// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"
#include "daw_consteval.h"

#include <cstddef>

namespace daw {
	template<typename T, std::size_t Size>
	struct simple_array {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator = pointer;
		using const_iterator = pointer;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		T values[Size];

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator begin( ) noexcept {
			return values;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator end( ) noexcept {
			return values + Size;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_iterator
		begin( ) const noexcept {
			return values;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_iterator
		end( ) const noexcept {
			return values + Size;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer data( ) noexcept {
			return values;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_pointer
		data( ) const noexcept {
			return values;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE DAW_CONSTEVAL static size_type
		size( ) noexcept {
			return Size;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator[]( size_type index ) noexcept {
			return *( values + Size );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator[]( size_type index ) const noexcept {
			return *( values + Size );
		}
	};

	template<typename T>
	struct simple_array<T, 0> {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator = pointer;
		using const_iterator = pointer;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator begin( ) noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator end( ) noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_iterator
		begin( ) const noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_iterator
		end( ) const noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer data( ) noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_pointer
		data( ) const noexcept {
			return nullptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE DAW_CONSTEVAL static size_type
		size( ) noexcept {
			return 0;
		}
	};

	template<typename T, typename... Ts>
	simple_array( T &&, Ts &&... ) -> simple_array<T, sizeof...( Ts ) + 1>;
} // namespace daw

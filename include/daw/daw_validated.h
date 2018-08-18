// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <type_traits>

#include "daw_traits.h"
#include "daw_utility.h"

/// Provide a class to validate a type T after construction
/// Validator must return true if the value is valid, false otherwise
namespace daw {
	template<typename T, typename Validator>
	struct validated {
		static_assert( is_same_v<T, remove_cvref_t<T>>,
		               "T cannot be cv or ref qualified" );
		// This is really true for ref qualified T as it allows the value to change
		// without validation

		using value_t = T;
		using reference = value_t &;
		using const_reference = value_t const &;

	private:
		value_t m_value;

		template<typename U>
		constexpr decltype( auto ) validate( U &&value ) {
			if( !Validator{}( value ) ) {
				throw std::out_of_range( "Argument did not pass validation" );
			}
			return std::forward<U>( value );
		}

	public:
		template<
		  typename... Args,
		  std::enable_if_t<( !is_enum_v<value_t> &&
		                     daw::is_constructible_v<value_t, Args...> &&
		                     !(sizeof...( Args ) == 1 &&
		                       daw::traits::is_first_type_v<validated, Args...>)),
		                   std::nullptr_t> = nullptr>
		constexpr validated( Args &&... args )
		  : m_value( validate(
		      construct_a<value_t>{}( std::forward<Args>( args )... ) ) ) {}

		// Handle enum's differently as it is UB to go out of range on a c enum
		template<typename Arg,
		         std::enable_if_t<(is_enum_v<value_t> &&
		                           !is_same_v<validated, std::decay_t<Arg>>),
		                          std::nullptr_t> = nullptr>
		constexpr validated( Arg &&arg )
		  : m_value(
		      static_cast<value_t>( validate( std::forward<Arg>( arg ) ) ) ) {}

		template<typename... Args,
		         std::enable_if_t<( is_enum_v<value_t> && sizeof...( Args ) == 0 ),
		                          std::nullptr_t> = nullptr>
		constexpr validated( Args&&... ) noexcept
		  : m_value{} {}

		constexpr const_reference get( ) const &noexcept {
			return m_value;
		}

		constexpr value_t &&get( ) && noexcept {
			return std::move( m_value );
		}

		constexpr operator const_reference( ) const &noexcept {
			return m_value;
		}

		constexpr operator value_t &&( ) && noexcept {
			return std::move( m_value );
		}
	};
} // namespace daw

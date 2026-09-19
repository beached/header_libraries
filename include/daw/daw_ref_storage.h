// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_move.h"

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	struct ref_storage {
		using type = T;
		using reference = std::remove_reference_t<type> &;
		using const_reference = std::remove_reference_t<type> const &;

	private:
		using storage_t = std::conditional_t<
		  std::is_rvalue_reference_v<T>, std::remove_cvref_t<T>,
		  std::conditional_t<std::is_reference_v<T>,
		                     std::add_pointer_t<std::remove_reference_t<T>>, T>>;
		storage_t m_storage{ };

		template<typename U>
		static constexpr bool can_store_v =
		  std::is_lvalue_reference_v<type>
		    ? std::is_lvalue_reference_v<U &&> and
		        std::convertible_to<std::add_pointer_t<std::remove_reference_t<U>>,
		                            storage_t>
		    : std::constructible_from<storage_t, U>;

		template<typename U>
		[[nodiscard]] static constexpr storage_t make_storage( U &&value ) {
			if constexpr( std::is_lvalue_reference_v<type> ) {
				return std::addressof( value );
			} else {
				return storage_t( DAW_FWD( value ) );
			}
		}

	public:
		ref_storage( ) = default;

		static constexpr bool is_owned = not std::is_lvalue_reference_v<T>;

		template<typename U>
		requires( can_store_v<U> ) explicit constexpr ref_storage( U &&value )
		  : m_storage( make_storage( DAW_FWD( value ) ) ) {}

		template<typename... Args>
		requires( not std::is_lvalue_reference_v<type> and
		          std::constructible_from<storage_t, Args...> ) //
		  explicit constexpr ref_storage( std::in_place_t, Args &&...args )
		  : m_storage( DAW_FWD( args )... ) {}

		template<typename U>
		requires( std::is_lvalue_reference_v<type> and can_store_v<U> ) //
		  explicit constexpr ref_storage( std::in_place_t, U &&value )
		  : m_storage( make_storage( DAW_FWD( value ) ) ) {}

		[[nodiscard]] constexpr bool good( ) const noexcept {
			if constexpr( std::is_lvalue_reference_v<type> ) {
				return static_cast<bool>( m_storage );
			} else {
				return true;
			}
		}

		[[nodiscard]] constexpr type get( ) && noexcept
		  requires( not std::is_lvalue_reference_v<type> ) {
			return std::move( m_storage );
		}

		[[nodiscard]] constexpr reference get( ) & noexcept {
			if constexpr( std::is_lvalue_reference_v<type> ) {
				daw_ensure( good( ) );
				return *m_storage;
			} else {
				return m_storage;
			}
		}

		[[nodiscard]] constexpr const_reference get( ) const & noexcept {
			if constexpr( std::is_lvalue_reference_v<type> ) {
				daw_ensure( good( ) );
				return *m_storage;
			} else {
				return m_storage;
			}
		}

		[[nodiscard]] constexpr const_reference get( ) const && noexcept {
			return as_const( *this ).get( );
		}

		[[nodiscard]] constexpr operator reference( ) noexcept {
			return get( );
		}

		[[nodiscard]] constexpr operator const_reference( ) const noexcept {
			return get( );
		}

		[[nodiscard]] constexpr bool operator==( ref_storage const &rhs ) const
		  requires( std::equality_comparable<type> ) {
			if( not good( ) ) {
				return not rhs.good( );
			}
			if( not rhs.good( ) ) {
				return false;
			}
			return get( ) == rhs.get( );
		}

		[[nodiscard]] constexpr bool operator==( const_reference rhs ) const
		  requires( std::equality_comparable<type> ) {
			if( not good( ) ) {
				return false;
			}
			return get( ) == rhs;
		}

		// clang-format off
		[[nodiscard]] constexpr auto operator<=>( ref_storage const &rhs )
		                              const requires( std::totally_ordered<type> ) {
			using result_t = DAW_TYPEOF( std::declval<const_reference>( ) <=>
			                             std::declval<const_reference>( ) );

			if( not good( ) ) {
				if( rhs.good( ) ) {
					return result_t::less;
				}
				return result_t::equal;
			}
			if( not rhs.good( ) ) {
				return result_t::greater;
			}
			return get( ) <=> rhs.get( );
		}

		[[nodiscard]] constexpr auto operator<=>( const_reference rhs )
		                              const requires( std::three_way_comparable<type> ) {
			using result_t = DAW_TYPEOF( std::declval<const_reference>( ) <=>
																 std::declval<const_reference>( ) );

			if( not good( ) ) {
				return result_t::less;
			}
			return get( ) <=> rhs;
		}
		// clang-format on
	};

	template<typename T>
	ref_storage( T && ) -> ref_storage<T>;
} // namespace daw

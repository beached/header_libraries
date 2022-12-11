// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_move.h"

#include <array>
#include <functional>
#include <limits>
#include <type_traits>

namespace daw {
	enum class keep_n_order { ascending, descending };
	namespace keep_n_impl {
		template<keep_n_order Order, typename Function>
		struct keep_n_pred {
			Function func = { };

			constexpr keep_n_pred( ) noexcept(
			  std::is_nothrow_constructible_v<Function> ) = default;
			constexpr keep_n_pred( Function const &f ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : func( f ) {}
			constexpr keep_n_pred( Function &&f ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : func( DAW_MOVE( f ) ) {}

			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&...args ) {
				if constexpr( Order == keep_n_order::ascending ) {
					return func( std::forward<Args>( args )... );
				} else {
					return not func( std::forward<Args>( args )... );
				}
			}

			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&...args ) const {
				if constexpr( Order == keep_n_order::ascending ) {
					return func( std::forward<Args>( args )... );
				} else {
					return not func( std::forward<Args>( args )... );
				}
			}
		};
	} // namespace keep_n_impl

	template<typename T, size_t MaxItems,
	         keep_n_order Order = keep_n_order::ascending,
	         typename Predicate = std::less<>>
	class keep_n {
		using values_type = std::array<T, MaxItems>;
		static constexpr auto const m_pred =
		  keep_n_impl::keep_n_pred<Order, Predicate>{ };

	public:
		using value_type = typename values_type::value_type;
		using difference_type = typename values_type::difference_type;
		using size_type = typename values_type::size_type;
		using reference = typename values_type::reference;
		using const_reference = typename values_type::const_reference;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;

	private:
		values_type m_values{ };

	public:
		constexpr keep_n( value_type const &default_value ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> ) {
			for( auto &v : m_values ) {
				v = default_value;
			}
		}

		constexpr void insert( value_type const &v ) {
			for( size_t n = 0; n < MaxItems; ++n ) {
				if( m_pred( v, m_values[n] ) ) {
					for( size_t m = ( MaxItems - 1U ); m > n; --m ) {
						m_values[m] = DAW_MOVE( m_values[m - 1] );
					}
					m_values[n] = v;
					break;
				}
			}
		}

		constexpr void insert( value_type &&v ) {
			for( size_t n = 0; n < MaxItems; ++n ) {
				if( m_pred( v, m_values[n] ) ) {
					for( size_t m = MaxItems - 1; m > n; --m ) {
						m_values[m] = DAW_MOVE( m_values[m - 1] );
					}
					m_values[n] = DAW_MOVE( v );
					break;
				}
			}
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type index ) const noexcept {
			return m_values[index];
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_values.begin( );
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_values.cbegin( );
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return m_values.end( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return m_values.cend( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return MaxItems;
		}

		[[nodiscard]] constexpr size_type capacity( ) const noexcept {
			return MaxItems;
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			return m_values.front( );
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			return m_values.back( );
		}
	};
} // namespace daw

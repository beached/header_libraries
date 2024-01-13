// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"
#include "../daw_container_traits.h"
#include "../daw_exception.h"
#include "../daw_traits.h"
#include "daw/traits/daw_traits_conditional.h"
#include "daw_arrow_proxy.h"

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw {
	template<typename T>
	struct iota_iterator {
		using value_type = T;
		using reference = T;
		using size_type =
		  typename conditional_t<std::is_integral_v<value_type>,
		                         std::make_unsigned<value_type>,
		                         daw::traits::identity<value_type>>::type;
		using difference_type =
		  typename conditional_t<std::is_integral_v<value_type>,
		                         std::make_signed<value_type>,
		                         daw::traits::identity<value_type>>::type;
		using pointer = daw::arrow_proxy<T>;
		using iterator_category = std::random_access_iterator_tag;

	private:
		value_type m_value = value_type{ 0 };
		difference_type m_step = difference_type{ 1 };

	public:
		explicit iota_iterator( ) = default;
		explicit constexpr iota_iterator( value_type start_value ) noexcept
		  : m_value( start_value ) {}

		explicit constexpr iota_iterator( value_type start_value,
		                                  difference_type step_distance ) noexcept
		  : m_value( start_value )
		  , m_step( step_distance ) {}

		constexpr iota_iterator &operator+=( difference_type n ) noexcept {
			m_value =
			  static_cast<value_type>( static_cast<difference_type>( m_value ) + n );
			return *this;
		}

		constexpr iota_iterator &operator-=( difference_type n ) noexcept {
			m_value =
			  static_cast<value_type>( static_cast<difference_type>( m_value ) - n );
			return *this;
		}

		[[nodiscard]] constexpr reference operator*( ) const noexcept {
			return m_value;
		}

		[[nodiscard]] constexpr pointer operator->( ) const noexcept {
			return daw::arrow_proxy<value_type>( m_value );
		}

		constexpr iota_iterator &operator++( ) noexcept {
			m_value = static_cast<value_type>( static_cast<size_type>( m_value ) +
			                                   static_cast<size_type>( m_step ) );
			return *this;
		}

		constexpr iota_iterator operator++( int ) noexcept {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr iota_iterator &operator--( ) noexcept {
			m_value = static_cast<value_type>( static_cast<size_type>( m_value ) -
			                                   static_cast<size_type>( m_step ) );
			return *this;
		}

		constexpr const iota_iterator operator--( int ) noexcept {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr iota_iterator operator+( difference_type n ) const noexcept {
			auto result = *this;
			result.m_value =
			  static_cast<value_type>( static_cast<size_type>( result.m_value ) +
			                           static_cast<size_type>( m_step * n ) );

			return result;
		}

		constexpr iota_iterator operator-( difference_type n ) const noexcept {
			auto result = *this;
			result.m_value =
			  static_cast<value_type>( static_cast<size_type>( result.m_value ) -
			                           static_cast<size_type>( m_step * n ) );

			return result;
		}

		constexpr difference_type
		operator-( iota_iterator const &rhs ) const noexcept {
			return static_cast<difference_type>( m_value - rhs.m_value ) / m_step;
		}

		constexpr reference operator[]( difference_type idx ) const noexcept {
			return *operator+( idx );
		}

		template<typename U>
		[[nodiscard]] constexpr difference_type
		compare( iota_iterator<U> const &rhs ) const noexcept {
			auto const diff = m_value - rhs.m_value;
			if( std::less<value_type>{ }( diff, value_type{ 0 } ) ) {
				return -1;
			}
			if( std::less<value_type>{ }( value_type{ 0 }, 0 ) ) {
				return 1;
			}
			return 0;
		}

		friend constexpr bool operator==( iota_iterator const &lhs,
		                                  iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) == 0;
		}

		friend constexpr bool operator!=( iota_iterator const &lhs,
		                                  iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) != 0;
		}

		friend constexpr bool operator>( iota_iterator const &lhs,
		                                 iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) > 0;
		}

		friend constexpr bool operator>=( iota_iterator const &lhs,
		                                  iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) >= 0;
		}

		friend constexpr bool operator<( iota_iterator const &lhs,
		                                 iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) < 0;
		}

		friend constexpr bool operator<=( iota_iterator const &lhs,
		                                  iota_iterator const &rhs ) noexcept {
			return lhs.compare( rhs ) < 0;
		}
	}; // iota_iterator

	iota_iterator( )->iota_iterator<int>;

	template<typename T, typename... Step>
	iota_iterator( T, Step... ) -> iota_iterator<T>;

	template<typename T>
	struct iota_range {
		using iterator = iota_iterator<T>;
		using value_type = typename iterator::value_type;
		using reference = typename iterator::reference;
		using size_type = typename iterator::size_type;
		using difference_type = typename iterator::difference_type;

	private:
		iterator m_first{ };
		iterator m_last{ };

	public:
		explicit iota_range( ) = default;
		explicit constexpr iota_range( reference last_value ) noexcept
		  : m_first( 0 )
		  , m_last( last_value ) {}

		explicit constexpr iota_range( reference start_value,
		                               reference last_value ) noexcept
		  : m_first( start_value )
		  , m_last( last_value ) {}

		explicit constexpr iota_range( reference start_value, reference last_value,
		                               difference_type step ) noexcept
		  : m_first( start_value, step )
		  , m_last( last_value ) {}

		constexpr iterator begin( ) const noexcept {
			return m_first;
		}

		constexpr iterator end( ) const noexcept {
			return m_last;
		}

		constexpr iterator cbegin( ) const noexcept {
			return m_first;
		}

		constexpr iterator cend( ) const noexcept {
			return m_last;
		}

		constexpr reference operator[]( difference_type idx ) const noexcept {
			return m_first[idx];
		}
	};
	iota_range( )->iota_range<int>;

	template<typename T, typename... EndStep>
	iota_range( T, EndStep... ) -> iota_range<T>;
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_exchange.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "impl/daw_make_trait.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace daw {
	namespace value_ptr_details {
		DAW_MAKE_REQ_TRAIT3(
		  is_comparable_v, std::declval<V /*Compare*/>( )( std::declval<T>( ),
		                                                   std::declval<U>( ) ) and
		                     std::declval<V /*Compare*/>( )( std::declval<U>( ),
		                                                     std::declval<T>( ) ) );
	} // namespace value_ptr_details

	template<typename T, typename Compare = std::less<>>
	struct value_ptr : enable_default_constructor<T>,
	                   enable_copy_constructor<T>,
	                   enable_copy_assignment<T> {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		std::unique_ptr<value_type> m_value{ };

	public:
		constexpr explicit value_ptr( std::nullopt_t ) noexcept {}

		template<
		  typename... Args,
		  std::enable_if_t<
		    all_true_v<std::is_constructible_v<value_type, Args...>,
		               !traits::is_first_type_v<value_ptr, std::decay_t<Args>...>>,
		    std::nullptr_t> = nullptr>
		explicit value_ptr( Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<value_type, Args...> )
		  : enable_default_constructor<T>( traits_details::non_constructor{ } )
		  , enable_copy_constructor<T>( traits_details::non_constructor{ } )
		  , enable_copy_assignment<T>( traits_details::non_constructor{ } )
		  , m_value( std::make_unique<value_type>( DAW_FWD( args )... ) ) {}

		template<
		  typename U, typename... Args,
		  std::enable_if_t<
		    all_true_v<std::is_constructible_v<value_type, Args...>,
		               !traits::is_first_type_v<value_ptr, std::decay_t<Args>...>>,
		    std::nullptr_t> = nullptr>
		static value_ptr emplace( Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<value_type, Args...> ) {

			auto result = value_ptr<value_type>( std::nullopt );
			result.m_value = std::make_unique<U>( DAW_FWD( args )... );
			return result;
		}

		value_ptr( value_ptr const &other ) noexcept(
		  noexcept( std::make_unique<value_type>( *other.m_value ) ) )
		  : enable_default_constructor<T>( other )
		  , enable_copy_constructor<T>( other )
		  , enable_copy_assignment<T>( other )
		  , m_value( std::make_unique<value_type>( *other.m_value ) ) {}

		constexpr value_ptr( value_ptr &&other ) noexcept
		  : enable_default_constructor<T>( std::move( other ) )
		  , enable_copy_constructor<T>( std::move( other ) )
		  , enable_copy_assignment<T>( std::move( other ) )
		  , m_value( std::move( other.m_value ) ) {}

		constexpr value_ptr &operator=( value_ptr const &rhs ) noexcept(
		  std::is_nothrow_copy_constructible_v<value_type> ) {
			if( this != rhs ) {
				if( !m_value and !rhs.m_value ) {
					return *this;
				}
				if( !rhs.m_value ) {
					reset( );
				} else if( !m_value ) {
					m_value = std::make_unique<value_type>( *rhs.m_value );
				} else {
					*m_value = *rhs.m_value;
				}
			}
			return *this;
		}

		constexpr value_ptr &operator=( value_ptr &&rhs ) noexcept {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
			}
			return *this;
		}

		value_ptr &operator=( value_type const &rhs ) noexcept(
		  std::is_nothrow_copy_assignable_v<value_type> ) {
			if( !m_value ) {
				m_value = std::make_unique<value_type>( rhs );
			} else {
				*m_value = rhs;
			}
			return *this;
		}

		value_ptr &operator=( value_type &&rhs ) noexcept(
		  std::is_nothrow_move_assignable_v<value_type> ) {
			*m_value = std::move( rhs );
			return *this;
		}

		constexpr void
		reset( ) noexcept( std::is_nothrow_destructible_v<value_type> ) {
			m_value.reset( );
		}

		template<typename... Args,
		         std::enable_if_t<std::is_invocable_v<value_type, Args...>,
		                          std::nullptr_t> = nullptr>
		decltype( auto ) operator( )( Args &&...args ) noexcept(
		  noexcept( m_value->operator( )( DAW_FWD( args )... ) ) ) {

			return m_value->operator( )( DAW_FWD( args )... );
		}

		~value_ptr( ) noexcept( std::is_nothrow_destructible_v<value_type> ) =
		  default;

		constexpr pointer release( ) noexcept {
			return daw::exchange( m_value, nullptr );
		}

		constexpr void swap( value_ptr &rhs ) noexcept {
			daw::cswap( m_value, rhs.m_value );
		}

		constexpr pointer get( ) noexcept {
			return m_value.get( );
		}

		constexpr const_pointer get( ) const noexcept {
			return m_value.get( );
		}

		reference operator*( ) noexcept {
			return *m_value;
		}

		const_reference operator*( ) const noexcept {
			return *m_value;
		}

		pointer operator->( ) noexcept {
			return m_value.get( );
		}

		const_pointer operator->( ) const noexcept {
			return m_value.get( );
		}

		explicit operator reference( ) noexcept {
			return *m_value;
		}

		explicit operator const_reference( ) const noexcept {
			return *m_value;
		}

		constexpr explicit operator bool( ) noexcept {
			return static_cast<bool>( m_value );
		}

		template<typename, typename>
		friend struct value_ptr;

		template<typename U>
		constexpr int compare( value_ptr<U> const &rhs ) const noexcept {
			if( !m_value ) {
				if( !rhs.m_value ) {
					return 0;
				}
				return -1;
			}
			if( !rhs.m_value ) {
				return 1;
			}
			if( Compare{ }( *m_value, *rhs.m_value ) ) {
				return -1;
			}
			if( Compare{ }( *rhs.m_value, *m_value ) ) {
				return 1;
			}
			return 0;
		}
	};

	template<typename T>
	constexpr void
	swap( value_ptr<T> &lhs,
	      value_ptr<T> &rhs ) noexcept( noexcept( lhs.swap( rhs ) ) ) {
		lhs.swap( rhs );
	}

	namespace value_ptr_details {
		DAW_MAKE_REQ_TRAIT2( has_compare_equality,
		                     std::declval<T>( ) == std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2( has_compare_inequality,
		                     std::declval<T>( ) != std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2( has_compare_greater,
		                     std::declval<T>( ) > std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2( has_compare_greater_equal,
		                     std::declval<T>( ) >= std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2( has_compare_less,
		                     std::declval<T>( ) < std::declval<U>( ) );

		DAW_MAKE_REQ_TRAIT2( has_compare_less_equal,
		                     std::declval<T>( ) <= std::declval<U>( ) );
	} // namespace value_ptr_details

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_equality<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator==( value_ptr<T> const &lhs,
	            value_ptr<U> const &rhs ) noexcept( noexcept( *lhs == *rhs ) ) {

		return lhs.compare( rhs ) == 0;
	}

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_inequality<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator!=( value_ptr<T> const &lhs,
	            value_ptr<U> const &rhs ) noexcept( noexcept( *lhs != *rhs ) ) {
		return lhs.compare( rhs ) != 0;
	}

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_greater<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator>( value_ptr<T> const &lhs,
	           value_ptr<U> const &rhs ) noexcept( noexcept( *lhs > *rhs ) ) {
		return lhs.compare( rhs ) > 0;
	}

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_greater_equal<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator>=( value_ptr<T> const &lhs,
	            value_ptr<U> const &rhs ) noexcept( noexcept( *lhs >= *rhs ) ) {
		return lhs.compare( rhs ) >= 0;
	}

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_less<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator<( value_ptr<T> const &lhs,
	           value_ptr<U> const &rhs ) noexcept( noexcept( *lhs < *rhs ) ) {
		return lhs.compare( rhs ) < 0;
	}

	template<typename T, typename U,
	         std::enable_if_t<value_ptr_details::has_compare_less_equal<T, U>,
	                          std::nullptr_t> = nullptr>
	constexpr bool
	operator<=( value_ptr<T> const &lhs,
	            value_ptr<U> const &rhs ) noexcept( noexcept( *lhs <= *rhs ) ) {
		return lhs.compare( rhs ) <= 0;
	}
	namespace value_ptr_details {
		struct empty_t {};
	} // namespace value_ptr_details
} // namespace daw

namespace std {
	template<typename T>
	struct hash<daw::value_ptr<T>>
	  : std::enable_if_t<std::is_invocable_v<std::hash<T>, T>,
	                     daw::value_ptr_details::empty_t> {

		template<typename Arg>
		size_t operator( )( Arg &&arg ) const {
			return std::hash<T>{ }( *DAW_FWD( arg ) );
		}
	};
} // namespace std

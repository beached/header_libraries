// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"

#include <atomic>
#include <concepts>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	concept AtomicValue = std::is_arithmetic_v<T>;

	template<typename>
	struct atomic;

	template<typename T>
	struct atomic<T const>;

	template<typename T>
	struct atomic<T const volatile>;

	template<typename T>
	struct atomic<T volatile>;

	template<AtomicValue T>
	struct atomic<T> {
		using value_type = T;
		using difference_type = T;

		static constexpr std::size_t required_alignment =
		  std::atomic_ref<value_type>::required_alignment;

		static constexpr bool is_always_lock_free =
		  std::atomic_ref<value_type>::is_always_lock_free;

	private:
		alignas( required_alignment ) value_type m_value = value_type{ };

	public:
		atomic( ) = default;
		~atomic( ) = default;

		constexpr atomic( value_type b ) noexcept
		  : m_value( b ) {}

		constexpr atomic &operator=( value_type b ) noexcept {
			store( b );
			return *this;
		}

		constexpr atomic( atomic const &other ) noexcept
		  : m_value( other.load( ) ) {}

		constexpr atomic( atomic &&other ) noexcept
		  : m_value( other.load( ) ) {}

		constexpr atomic &operator=( atomic const &rhs ) noexcept {
			if( this != &rhs ) {
				store( rhs.load( ) );
			}
			return *this;
		}

		constexpr atomic &operator=( atomic &&rhs ) noexcept {
			if( this != &rhs ) {
				store( rhs.load( ) );
			}
			return *this;
		}

		DAW_ATTRIB_FLATINLINE constexpr void
		store( this auto &self, value_type b,
		       std::memory_order m = std::memory_order::seq_cst ) noexcept {
			if consteval {
				(void)m;
				self.m_value = b;
			} else {
				std::atomic_ref<value_type>( self.m_value ).store( b, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		load( std::memory_order m = std::memory_order_seq_cst ) const noexcept {
			if consteval {
				(void)m;
				return m_value;
			} else {
				return std::atomic_ref<value_type>(
				         const_cast<value_type &>( m_value ) )
				  .load( m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		exchange( this auto &self, value_type b,
		          std::memory_order m = std::memory_order_seq_cst ) noexcept {
			if consteval {
				return std::exchange( self.m_value, b );
			} else {
				return std::atomic_ref<value_type>( self.m_value ).exchange( b, m );
			}
		}

		DAW_ATTRIB_FLATINLINE bool is_lock_free( ) const noexcept {
			return std::atomic_ref<value_type>( const_cast<value_type &>( m_value ) )
			  .is_lock_free( );
		}

		DAW_ATTRIB_FLATINLINE constexpr
		operator value_type( this auto const &self ) noexcept {
			return self.load( );
		}

		DAW_ATTRIB_FLATINLINE constexpr bool
		compare_exchange_weak( this auto &self, value_type &expected,
		                       value_type desired, std::memory_order success,
		                       std::memory_order failure ) noexcept {
			if consteval {
				if( self.m_value == expected ) {
					self.m_value = desired;
					return true;
				}
				expected = self.m_value;
				return false;
			} else {
				return std::atomic_ref<value_type>( self.m_value )
				  .compare_exchange_weak( expected, desired, success, failure );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr bool compare_exchange_weak(
		  this auto &self, value_type &expected, value_type desired,
		  std::memory_order order = std::memory_order::seq_cst ) noexcept {
			if consteval {
				if( self.m_value == expected ) {
					self.m_value = desired;
					return true;
				}
				expected = self.m_value;
				return false;
			} else {
				return std::atomic_ref<value_type>( self.m_value )
				  .compare_exchange_weak( expected, desired, order );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr bool
		compare_exchange_strong( this auto &self, value_type &expected,
		                         value_type desired, std::memory_order success,
		                         std::memory_order failure ) noexcept {
			if consteval {
				if( self.m_value == expected ) {
					self.m_value = desired;
					return true;
				}
				expected = self.m_value;
				return false;
			} else {
				return std::atomic_ref<value_type>( self.m_value )
				  .compare_exchange_strong( expected, desired, success, failure );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr bool compare_exchange_strong(
		  this auto &self, value_type &expected, value_type desired,
		  std::memory_order order = std::memory_order::seq_cst ) noexcept {
			if consteval {
				if( self.m_value == expected ) {
					self.m_value = desired;
					return true;
				}
				expected = self.m_value;
				return false;
			} else {
				return std::atomic_ref<value_type>( self.m_value )
				  .compare_exchange_strong( expected, desired, order );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		fetch_add( this auto &self, value_type arg,
		           std::memory_order m = std::memory_order::seq_cst ) noexcept
		  requires( not std::same_as<bool, value_type> ) {
			if consteval {
				(void)m;
				auto result = self.m_value;
				self.m_value += arg;
				return result;
			} else {
				return std::atomic_ref<value_type>( self.m_value ).fetch_add( arg, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		fetch_sub( this auto &self, value_type arg,
		           std::memory_order m = std::memory_order::seq_cst ) noexcept
		  requires( not std::same_as<bool, value_type> ) {
			if consteval {
				(void)m;
				auto result = self.m_value;
				self.m_value -= arg;
				return result;
			} else {
				return std::atomic_ref<value_type>( self.m_value ).fetch_sub( arg, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		operator+=( this auto &self, value_type arg ) noexcept
		  requires( not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value += arg;
			} else {
				return std::atomic_ref<value_type>( self.m_value ) += arg;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		operator-=( this auto &self, value_type arg ) noexcept
		  requires( not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value -= arg;
			} else {
				return std::atomic_ref<value_type>( self.m_value ) -= arg;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		operator++( this auto &self ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return ++self.m_value;
			} else {
				return ++std::atomic_ref<value_type>( self.m_value );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type operator++( this auto &self,
		                                                       int ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value++;
			} else {
				return std::atomic_ref<value_type>( self.m_value )++;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		operator--( this auto &self ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return --self.m_value;
			} else {
				return --std::atomic_ref<value_type>( self.m_value );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type operator--( this auto &self,
		                                                       int ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value--;
			} else {
				return std::atomic_ref<value_type>( self.m_value )--;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		fetch_and( this auto &self, value_type arg,
		           std::memory_order m = std::memory_order::seq_cst ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				(void)m;
				auto result = self.m_value;
				self.m_value &= arg;
				return result;
			} else {
				return std::atomic_ref<value_type>( self.m_value ).fetch_and( arg, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		fetch_or( this auto &self, value_type arg,
		          std::memory_order m = std::memory_order::seq_cst ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				(void)m;
				auto result = self.m_value;
				self.m_value |= arg;
				return result;
			} else {
				return std::atomic_ref<value_type>( self.m_value ).fetch_or( arg, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type
		fetch_xor( this auto &self, value_type arg,
		           std::memory_order m = std::memory_order::seq_cst ) noexcept
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				(void)m;
				auto result = self.m_value;
				self.m_value ^= arg;
				return result;
			} else {
				return std::atomic_ref<value_type>( self.m_value ).fetch_xor( arg, m );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type operator&=( this auto &self,
		                                                       value_type arg )
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value &= arg;
			} else {
				return std::atomic_ref<value_type>( self.m_value ) &= arg;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type operator|=( this auto &self,
		                                                       value_type arg )
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value |= arg;
			} else {
				return std::atomic_ref<value_type>( self.m_value ) |= arg;
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr value_type operator^=( this auto &self,
		                                                       value_type arg )
		  requires( std::is_integral_v<value_type> and
		            not std::same_as<bool, value_type> ) {
			if consteval {
				return self.m_value ^= arg;
			} else {
				return std::atomic_ref<value_type>( self.m_value ) ^= arg;
			}
		}
	};
} // namespace daw

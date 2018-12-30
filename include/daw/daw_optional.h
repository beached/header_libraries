// The MIT License (MIT)
//
// Copyright (c) 2016-2018 Darrell Wright
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

#include <array>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "daw_exception.h"
#include "daw_swap.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename T>
		struct value_storage {
			using value_type = T;
			using pointer = value_type *;
			using const_pointer = value_type const *;
			using reference = value_type &;
			using const_reference = value_type const &;

		private:
			std::array<uint8_t, sizeof( T )> m_data = {0};
			bool m_occupied = false;

			void *raw_ptr( ) noexcept {
				return static_cast<void *>( m_data.data( ) );
			}

			void const *raw_ptr( ) const noexcept {
				return static_cast<void const *>( m_data.data( ) );
			}

			pointer ptr( ) noexcept {
				return static_cast<pointer>( raw_ptr( ) );
			}

			const_pointer ptr( ) const noexcept {
				return static_cast<const_pointer>( raw_ptr( ) );
			}

			reference ref( ) noexcept {
				return *ptr( );
			}

			const_reference ref( ) const noexcept {
				return *ptr( );
			}

			void store( value_type value ) {
				if( m_occupied ) {
					*ptr( ) = std::move( value );
				} else {
					m_occupied = true;
					new( raw_ptr( ) ) value_type{std::move( value )};
				}
			}

			template<typename... Args>
			void create( Args &&... args ) {
				if( m_occupied ) {
					reset( );
				} else {
					m_occupied = true;
					new( raw_ptr( ) ) value_type{std::forward<Args>( args )...};
				}
			}

		public:
			struct default_construct {};

			value_storage( ) noexcept = default;

			value_storage( default_construct )
			  : m_data( 0 )
			  , m_occupied( true ) {
				store( value_type( ) );
			}

			template<typename... Args>
			void emplace( Args &&... args ) {
				create( std::forward<Args>( args )... );
			}

			value_storage( value_type value )
			  : m_data{0}
			  , m_occupied( true ) {
				store( std::move( value ) );
			}

			value_storage( value_storage const &other ) noexcept
			  : m_data( other.m_data )
			  , m_occupied( other.m_occupied ) {}

			value_storage( value_storage &&other ) noexcept
			  : m_data( std::move( other.m_data ) )
			  , m_occupied( std::exchange( other.m_occupied, false ) ) {}

			void swap( value_storage &rhs ) noexcept {
				daw::cswap( m_data, rhs.m_data );
				daw::cswap( m_occupied, rhs.m_occupied );
			}

			value_storage &operator=( value_storage const &rhs ) {
				if( this != &rhs ) {
					std::copy( rhs.m_data.cbegin( ), rhs.m_data.cend( ),
					           m_data.begin( ) );
					m_occupied = rhs.m_occupied;
				}
				return *this;
			}

			value_storage &operator=( value_storage &&rhs ) noexcept {
				m_occupied = false;
				m_data = std::move( rhs.m_data );
				m_occupied = std::exchange( rhs.m_occupied, false );
				return *this;
			}

			value_storage &operator=( value_type value ) {
				if( m_occupied ) {
					m_occupied = false;
					*ptr( ) = std::move( value );
					m_occupied = true;
				} else {
					m_occupied = false;
					store( std::move( value ) );
					m_occupied = true;
				}
				return *this;
			}

			~value_storage( ) {
				reset( );
			}

			void reset( ) {
				if( m_occupied ) {
					m_occupied = false;
					ref( ).~value_type( );
				}
			}

			bool empty( ) const noexcept {
				return !m_occupied;
			}

			explicit operator bool( ) const noexcept {
				return m_occupied;
			}

			reference operator*( ) {
				daw::exception::daw_throw_on_true( empty( ),
				                                   "Attempt to access an empty value" );
				return ref( );
			}

			const_reference operator*( ) const {
				daw::exception::daw_throw_on_true( empty( ),
				                                   "Attempt to access an empty value" );
				return ref( );
			}
		}; // value_storage

		template<typename T>
		void swap( value_storage<T> &lhs, value_storage<T> &rhs ) noexcept {
			lhs.swap( rhs );
		}
	} // namespace impl

	template<class ValueType>
	struct optional {
		using value_type = std::remove_cv_t<std::remove_reference_t<ValueType>>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using pointer_const = value_type const *;

	private:
		impl::value_storage<value_type> m_value{};

	public:
		optional( nothing )
		  : m_value{} {}

		explicit optional( value_type value )
		  : m_value{std::move( value )} {}

		optional( ) = default;
		optional( optional const & ) = default;
		optional( optional && ) noexcept = default;
		optional &operator=( optional const & ) = default;
		optional &operator=( optional &&rhs ) noexcept = default;

		template<typename... Args>
		void emplace( Args &&... args ) {
			m_value = value_type{std::forward<Args>( args )...};
		}

		optional &operator=( nothing && ) noexcept {
			reset( );
			return *this;
		}

		void swap( optional &rhs ) noexcept {
			m_value.swap( rhs.m_value );
		}

		optional &operator=( value_type value ) {
			m_value = std::move( value );
			return *this;
		}

		optional &operator=( nothing ) {
			reset( );
			return *this;
		}

		~optional( ) = default;

		bool empty( ) const noexcept {
			return m_value.empty( );
		}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		explicit operator bool( ) const noexcept {
			return has_value( );
		}

		reference get( ) {
			return *m_value;
		}

		const_reference get( ) const {
			return *m_value;
		}

		reference operator*( ) {
			return *m_value;
		}

		const_reference operator*( ) const {
			return *m_value;
		}

		pointer operator->( ) {
			return m_value;
		}

		pointer_const operator->( ) const {
			return m_value;
		}

		void reset( ) {
			m_value.reset( );
		}

		template<typename T>
		friend bool operator==( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_inequality_comparable_v<value_type, T>,
			               "Types are not equality comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) == rhs.get( );
				}
				return false;
			}
			if( rhs ) {
				return false;
			}
			return true;
		}

		template<typename T>
		friend bool operator==( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_inequality_comparable_v<value_type, T>,
			               "Types are not equality comparable" );
			if( lhs ) {
				return lhs.get( ) == rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator!=( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_inequality_comparable_v<value_type, T>,
			               "Types are not inequality comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) != rhs.get( );
				}
				return true;
			}
			if( rhs ) {
				return true;
			}
			return false;
		}

		template<typename T>
		friend bool operator!=( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_inequality_comparable_v<value_type, T>,
			               "Types are not inequality comparable" );
			if( lhs ) {
				return lhs.get( ) != rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator<( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_less_than_comparable_v<value_type, T>,
			               "Types are not less than comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) < rhs.get( );
				}
				return false;
			}
			if( rhs ) {
				return true;
			}
			return false;
		}

		template<typename T>
		friend bool operator<( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_less_than_comparable_v<value_type, T>,
			               "Types are not less than comparable" );
			if( lhs ) {
				return lhs.get( ) < rhs;
			}
			return true;
		}

		template<typename T>
		friend bool operator>( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_greater_than_comparable_v<value_type, T>,
			               "Types are not greater than comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) > rhs.get( );
				}
				return true;
			}
			if( rhs ) {
				return false;
			}
			return false;
		}

		template<typename T>
		friend bool operator>( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_greater_than_comparable_v<value_type, T>,
			               "Types are not greater than comparable" );
			if( lhs ) {
				return lhs.get( ) > rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator<=( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_equal_less_than_comparable_v<value_type, T>,
			               "Types are not equal_less than comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) <= rhs.get( );
				}
				return false;
			}
			if( rhs ) {
				return true;
			}
			return true;
		}

		template<typename T>
		friend bool operator<=( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_equal_less_than_comparable_v<value_type, T>,
			               "Types are not equal_less than comparable" );
			if( lhs ) {
				return lhs.get( ) <= rhs;
			}
			return true;
		}

		template<typename T>
		friend bool operator>=( optional const &lhs, optional<T> const &rhs ) {
			static_assert( traits::is_equal_greater_than_comparable_v<value_type, T>,
			               "Types are not equal_greater than comparable" );
			if( lhs ) {
				if( rhs ) {
					return lhs.get( ) >= rhs.get( );
				}
				return true;
			}
			if( rhs ) {
				return false;
			}
			return true;
		}

		template<typename T>
		friend bool operator>=( optional const &lhs, T const &rhs ) {
			static_assert( traits::is_equal_greater_than_comparable_v<value_type, T>,
			               "Types are not equal_greater than comparable" );
			if( lhs ) {
				return lhs.get( ) >= rhs;
			}
			return false;
		}

	}; // class optional

	template<typename T>
	void swap( optional<T> &lhs, optional<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename T, typename... Args>
	auto make_optional( Args &&... args ) {
		optional<T> result{};
		result.emplace( std::forward<Args>( args )... );
		return result;
	}

	template<typename T, typename U>
	bool operator!=( optional<T> const &lhs, U const &rhs ) {
		static_assert( traits::is_inequality_comparable_v<T, U>,
		               "Types are not inequality comparable" );
		if( lhs ) {
			return lhs != rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator<( optional<T> const &lhs, U const &rhs ) {
		static_assert( traits::is_inequality_comparable_v<T, U>,
		               "Types are not less than comparable" );
		if( lhs ) {
			return lhs < rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator>( optional<T> const &lhs, U const &rhs ) {
		static_assert( traits::is_inequality_comparable_v<T, U>,
		               "Types are not greater than comparable" );
		if( lhs ) {
			return lhs > rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator<=( optional<T> const &lhs, U const &rhs ) {
		static_assert( traits::is_inequality_comparable_v<T, U>,
		               "Types are not equal less than comparable" );
		if( lhs ) {
			return lhs <= rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator>=( optional<T> const &lhs, U const &rhs ) {
		static_assert( traits::is_inequality_comparable_v<T, U>,
		               "Types are not equal greater than comparable" );
		if( lhs ) {
			return lhs >= rhs;
		}
		return false;
	}
} // namespace daw

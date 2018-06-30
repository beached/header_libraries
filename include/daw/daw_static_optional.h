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
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename T>
		union static_optional_storage {
			static_assert( daw::is_nothrow_destructible_v<T> || daw::is_trivial_v<T>,
			               "static_optional_storage can only be used for types that "
			               "are nothrow destructable or trivial" );

			T value;
			daw::nothing empty_value;

			constexpr static_optional_storage( daw::nothing ) noexcept
			  : empty_value{} {}
			constexpr static_optional_storage( T v ) noexcept
			  : value{std::move( v )} {}
		};
	} // namespace impl

	template<typename Value>
	struct static_optional {
		static_assert( daw::is_nothrow_destructible_v<Value> ||
		                 daw::is_trivial_v<Value>,
		               "static_optional can only be used for types that are "
		               "nothrow destructable or trivial" );

		using value_type = Value;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		impl::static_optional_storage<value_type> m_value;
		bool m_occupied;

		constexpr reference ref( ) {
			daw::exception::daw_throw_on_true( empty( ),
			                                   "Attempt to access an empty value" );
			return m_value.value;
		}

		constexpr const_reference ref( ) const {
			daw::exception::daw_throw_on_true( empty( ),
			                                   "Attempt to access an empty value" );
			return m_value.value;
		}

	public:
		constexpr static_optional( ) noexcept
		  : m_value{daw::nothing{}}
		  , m_occupied{false} {}
		explicit constexpr static_optional( daw::nothing ) noexcept
		  : m_value{daw::nothing{}}
		  , m_occupied{false} {}

		template<
		  typename Arg, typename... Args,
		  std::enable_if_t<!daw::is_same_v<static_optional, std::decay_t<Arg>>,
		                   std::nullptr_t> = nullptr>
		constexpr static_optional( Arg &&arg,
		                           Args &&... args ) noexcept( noexcept( value_type{
		  std::forward<Arg>( arg ), std::forward<Args>( args )...} ) )

		  : m_value{value_type{std::forward<Arg>( arg ),
		                       std::forward<Args>( args )...}}
		  , m_occupied{true} {}

		~static_optional( ) noexcept = default;

		constexpr static_optional( static_optional const &other ) noexcept
		  : m_value{other.m_value}
		  , m_occupied{other.m_occupied} {}

		constexpr static_optional( static_optional &&other ) noexcept
		  : m_value{std::move( other.m_value )}
		  , m_occupied{daw::exchange( other.m_occupied, false )} {}

		constexpr static_optional &
		operator=( static_optional const &rhs ) noexcept {
			if( &rhs != this ) {
				m_value = rhs.m_value;
				m_occupied = rhs.m_occupied;
			}
			return *this;
		}

		constexpr static_optional &operator=( static_optional &&rhs ) noexcept {
			m_value = std::move( rhs.m_value );
			m_occupied = daw::exchange( rhs.m_occupied, false );
			return *this;
		}

		constexpr static_optional &operator=( daw::nothing ) noexcept {
			m_value = daw::nothing{};
			m_occupied = false;
			return *this;
		}

		template<typename T, std::enable_if_t<
		                       !daw::is_same_v<static_optional, std::decay_t<T>> &&
		                         !daw::is_same_v<daw::nothing, std::decay_t<T>>,
		                       std::nullptr_t> = nullptr>
		constexpr static_optional &operator=( T value ) noexcept {
			m_value = std::move( value );
			m_occupied = true;
			return *this;
		}

		constexpr bool empty( ) const noexcept {
			return !m_occupied;
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_occupied;
		}

		constexpr operator value_type( ) const {
			return ref( );
		}

		constexpr reference operator*( ) {
			return ref( );
		}

		const_reference operator*( ) const {
			daw::exception::daw_throw_on_true( empty( ),
			                                   "Attempt to access an empty value" );
			return ref( );
		}

		constexpr bool has_value( ) const noexcept {
			return !m_occupied;
		}

		constexpr reference get( ) {
			return ref( );
		}

		constexpr const_reference get( ) const {
			return ref( );
		}

		template<typename T>
		friend bool operator==( static_optional const &lhs,
		                        static_optional<T> const &rhs ) {
			static_assert( daw::is_inequality_comparable_v<value_type, T>,
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
		friend bool operator==( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_inequality_comparable_v<value_type, T>,
			               "Types are not equality comparable" );
			if( lhs ) {
				return lhs.get( ) == rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator!=( static_optional const &lhs,
		                        static_optional<T> const &rhs ) {
			static_assert( daw::is_inequality_comparable_v<value_type, T>,
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
		friend bool operator!=( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_inequality_comparable_v<value_type, T>,
			               "Types are not inequality comparable" );
			if( lhs ) {
				return lhs.get( ) != rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator<( static_optional const &lhs,
		                       static_optional<T> const &rhs ) {
			static_assert( daw::is_less_than_comparable_v<value_type, T>,
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
		friend bool operator<( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_less_than_comparable_v<value_type, T>,
			               "Types are not less than comparable" );
			if( lhs ) {
				return lhs.get( ) < rhs;
			}
			return true;
		}

		template<typename T>
		friend bool operator>( static_optional const &lhs,
		                       static_optional<T> const &rhs ) {
			static_assert( daw::is_greater_than_comparable_v<value_type, T>,
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
		friend bool operator>( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_greater_than_comparable_v<value_type, T>,
			               "Types are not greater than comparable" );
			if( lhs ) {
				return lhs.get( ) > rhs;
			}
			return false;
		}

		template<typename T>
		friend bool operator<=( static_optional const &lhs,
		                        static_optional<T> const &rhs ) {
			static_assert( daw::is_equal_less_than_comparable_v<value_type, T>,
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
		friend bool operator<=( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_equal_less_than_comparable_v<value_type, T>,
			               "Types are not equal_less than comparable" );
			if( lhs ) {
				return lhs.get( ) <= rhs;
			}
			return true;
		}

		template<typename T>
		friend bool operator>=( static_optional const &lhs,
		                        static_optional<T> const &rhs ) {
			static_assert( daw::is_equal_greater_than_comparable_v<value_type, T>,
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
		friend bool operator>=( static_optional const &lhs, T const &rhs ) {
			static_assert( daw::is_equal_greater_than_comparable_v<value_type, T>,
			               "Types are not equal_greater than comparable" );
			if( lhs ) {
				return lhs.get( ) >= rhs;
			}
			return false;
		}

	}; // namespace daw

	template<typename T>
	void swap( static_optional<T> &lhs, static_optional<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename T, typename... Args>
	auto make_optional( Args &&... args ) {
		static_optional<T> result{};
		result.emplace( std::forward<Args>( args )... );
		return result;
	}

	template<typename T, typename U>
	bool operator!=( static_optional<T> const &lhs, U const &rhs ) {
		static_assert( daw::is_inequality_comparable_v<T, U>,
		               "Types are not inequality comparable" );
		if( lhs ) {
			return lhs != rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator<( static_optional<T> const &lhs, U const &rhs ) {
		static_assert( daw::is_inequality_comparable_v<T, U>,
		               "Types are not less than comparable" );
		if( lhs ) {
			return lhs < rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator>( static_optional<T> const &lhs, U const &rhs ) {
		static_assert( daw::is_inequality_comparable_v<T, U>,
		               "Types are not greater than comparable" );
		if( lhs ) {
			return lhs > rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator<=( static_optional<T> const &lhs, U const &rhs ) {
		static_assert( daw::is_inequality_comparable_v<T, U>,
		               "Types are not equal less than comparable" );
		if( lhs ) {
			return lhs <= rhs;
		}
		return false;
	}

	template<typename T, typename U>
	bool operator>=( static_optional<T> const &lhs, U const &rhs ) {
		static_assert( daw::is_inequality_comparable_v<T, U>,
		               "Types are not equal greater than comparable" );
		if( lhs ) {
			return lhs >= rhs;
		}
		return false;
	}
} // namespace daw

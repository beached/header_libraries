// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <type_traits>
#include <cstdlib>
#include <utility>
#include <functional>

namespace daw {
	template<typename Value, typename AddressType=size_t>
	struct memory {
		static_assert( std::is_integral<AddressType>::value, "address_t must be an integral type" );
		using value_type = typename std::add_volatile_t<std::remove_volatile_t<Value>>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = AddressType;
	private:
		uintptr_t m_ptr;

		constexpr pointer get( ) noexcept {
			return reinterpret_cast<pointer>(m_ptr);
		}

		constexpr const_pointer get( ) const noexcept {
			return reinterpret_cast<const_pointer>(m_ptr);
		}
	public:
		
		memory( ) = delete;
		~memory( ) = default;
		
		constexpr memory( memory const & other ) noexcept: m_ptr{ other.m_ptr } { }
		
		constexpr memory( memory && other ) noexcept : m_ptr{ std::exchange( other.m_ptr, 0 ) } { }
		
		constexpr memory & operator=( memory const & rhs ) noexcept {
			if( this != &rhs ) {
				memory tmp( rhs );
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		constexpr memory & operator=( memory && rhs ) noexcept {
			if( this != &rhs ) {
				memory tmp( std::move( rhs ) );
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		constexpr memory( uintptr_t location ): m_ptr{ location } { }

		friend void swap( memory & lhs, memory & rhs ) noexcept {
			using std::swap;
			swap( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr explicit operator bool( ) const noexcept {
			return 0 != m_ptr;
		}

		constexpr reference operator->( ) noexcept {
			return *get( );
		}

		constexpr const_reference operator->( ) const noexcept {
			return *get( );
		}

		constexpr pointer data( ) noexcept {
			return get( );
		}

		constexpr const_pointer data( ) const noexcept {
			return get( );
		}
		
		constexpr const_reference operator()( ) const noexcept {
			return *get( );
		}

		constexpr reference operator()( ) noexcept {
			return *get( );
		}

		constexpr reference operator[]( size_type offset ) noexcept {
			return *(get( ) + offset);
		}

		constexpr const_reference operator[]( size_type offset ) const noexcept {
			return *(get( ) + offset);
		}

		constexpr friend bool operator==( memory const & lhs, memory const & rhs ) noexcept {
			return std::equal_to<>( )( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr friend bool operator!=( memory const & lhs, memory const & rhs ) noexcept {
			return !std::equal_to<>( )( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr friend bool operator<( memory const & lhs, memory const & rhs ) noexcept {
			return std::less<>( )( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr friend bool operator>( memory const & lhs, memory const & rhs ) noexcept {
			return std::greater<>( )( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr friend bool operator<=( memory const & lhs, memory const & rhs ) noexcept {
			return std::less_equal<>( )( lhs.m_ptr, rhs.m_ptr );
		}

		constexpr friend bool operator>=( memory const & lhs, memory const & rhs ) noexcept {
			return std::greater_equal<>( )( lhs.m_ptr, rhs.m_ptr );
		}


	};
}

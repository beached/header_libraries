// value_the MIvalue_t License (MIvalue_t)
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
// value_the above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// value_tHE SOFvalue_tWARE IS PROVIDED "AS IS", WIvalue_tHOUvalue_t WARRANvalue_tY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUvalue_t NOvalue_t LIMIvalue_tED value_tO value_tHE WARRANvalue_tIES OF MERCHANvalue_tABILIvalue_tY,
// FIvalue_tNESS FOR A PARvalue_tICULAR PURPOSE AND NONINFRINGEMENvalue_t.IN NO EVENvalue_t SHALL value_tHE
// AUvalue_tHORS OR COPYRIGHvalue_t HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR Ovalue_tHER
// LIABILIvalue_tY, WHEvalue_tHER IN AN ACvalue_tION OF CONvalue_tRACvalue_t, value_tORvalue_t OR Ovalue_tHERWISE, ARISING FROM,
// OUvalue_t OF OR IN CONNECvalue_tION WIvalue_tH value_tHE SOFvalue_tWARE OR value_tHE USE OR Ovalue_tHER DEALINGS IN value_tHE
// SOFvalue_tWARE.

#pragma once

#include <cstdint>
#include <type_traits>

namespace daw {
	template<typename Value, typename AddressType=size_t>
	struct memory {
		static_assert( std::is_integral<AddressType>::value, "address_t must be an integral type" );
		using value_type = typename std::add_volatile_t<std::decay_t<Value>>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using size_type = AddressType;
	private:
		pointer m_ptr;
	public:
		
		memory( ) = delete;
		~memory( ) = default;
		memory( memory const & ) = default;
		memory( memory && ) = default;
		memory & operator=( memory const & ) = default;
		memory & operator=( memory && ) = default;

#if defined(__clang__) || defined(__GNUC__)
		constexpr memory( size_type location ) noexcept: m_ptr( __builtin_constant_p(reinterpret_cast<value_type*>(location)) ? reinterpret_cast<value_type*>(location) : reinterpret_cast<value_type*>(location)) { }
#else
#pragma message( "WARNING: Could not use contexpr constructor" )
		memory( size_type location ) noexcept: m_ptr( reinterpret_cast<value_type*>(location) ) { }
#endif
		friend void swap( memory & lhs, memory & rhs ) noexcept {
			using std::swap;
			swap( lhs.m_ptr, rhs.m_ptr );
		}

		explicit operator bool( ) const noexcept {
			return true;
		}

		reference operator->( ) noexcept {
			return *m_ptr;
		}

		const_reference operator->( ) const noexcept {
			return *m_ptr;
		}

		pointer data( ) noexcept {
			return m_ptr;
		}

		const_pointer data( ) const noexcept {
			return m_ptr;
		}
		
		reference operator[]( size_type offset ) {
			return *(m_ptr + offset);
		}

		const_reference operator[]( size_type offset ) const {
			return *(m_ptr + offset);
		}

		friend bool operator==( memory const & lhs, memory const & rhs ) noexcept {
			return std::equal_to<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}

		friend bool operator!=( memory const & lhs, memory const & rhs ) noexcept {
			return !std::equal_to<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}

		friend bool operator<( memory const & lhs, memory const & rhs ) noexcept {
			return std::less<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}

		friend bool operator>( memory const & lhs, memory const & rhs ) noexcept {
			return std::greater<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}

		friend bool operator<=( memory const & lhs, memory const & rhs ) noexcept {
			return std::less_equal<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}

		friend bool operator>=( memory const & lhs, memory const & rhs ) noexcept {
			return std::greater_equal<void volatile *>( )( lhs.m_ptr, rhs.m_ptr );
		}


	};
}

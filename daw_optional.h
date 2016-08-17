// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include "daw_traits.h"

namespace daw {
	template<class ValueType>
	struct optional {
		using value_type = std::decay_t<std::remove_reference_t<ValueType>>;
		using reference = ValueType &;
		using const_reference = ValueType const &;
		using pointer = ValueType *;
	private:
		pointer m_value;
	public:
		optional( ):
			m_value{ nullptr } { }

		optional( optional const & other ): m_value{ new value_type( *other.m_value ) } { }

		optional & operator=( optional const & rhs ) {
			if( this != &rhs ) {
				using std::swap;
				Option tmp{ rhs };
				swap( *this, tmp );
			}
			return *this;
		}

		optional( optional&& other ): 
				m_value{ std::exchange( other.m_value, nullptr ) { }

		optional & operator=( optional && rhs ) {
			if( this != &rhs ) {
				optional tmp{ std::move( rhs ) };
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		~optional( ) = default;

		friend void swap( optional & lhs, optional & rhs ) noexcept {
			using std::swap;
			swap( lhs.m_value, rhs.m_value );
		}

		bool operator==( optional const & rhs ) const noexcept {
			return rhs.m_value == m_value;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		optional( value_type value ):
				m_value{ new value_type( std::move( value ) ) } { }

		optional & operator=( value_type value ) {
			Option tmp{ std::move( value ) };
			using std::swap;
			swap( *this, tmp );
			return *this;
		}

		bool has_value( ) const noexcept {
			return nullptr != m_value;
		}

		explicit operator bool( ) const noexcept {
			return has_value( );
		}

		reference get( ) {
			assert( nullptr != m_value );
			return *m_value;
		}

		const_reference get( ) const {
			assert( nullptr != m_value );
			return *m_value;
		}

		reference operator*( ) {
			return *get( );
		}

		const_reference operator*( ) const {
			return *get( );
		}

		reference operator->( ) {
			return *get( );
		}

		const_reference operator->( ) const {
			return *get( );
		}

		value_type move_out( ) {
			assert( nullptr != m_value );
			value_type tmp = std::move( *m_value );
			m_value = nullptr;
			return tmp;
		}
	};	// class optional
	static_assert(::daw::traits::is_regular<optional<int>>::value, "optional isn't regular");
}	// namespace daw


// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
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

#include <boost/optional.hpp>
#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <typeindex>
#include <typeinfo>

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_traits.h"

namespace daw {
	struct bad_variant_t_access: public std::runtime_error {
		template<typename... Args>
		bad_variant_t_access( Args&&... args ):
				std::runtime_error{ std::forward<Args>( args )... } { }

		~bad_variant_t_access( ) = default;
	};

	template<typename Type, typename... Types>
	struct variant_t {
		static constexpr size_t BUFFER_SIZE = daw::traits::max_sizeof_v<Type,Types...>;
		template<typename T>
		static constexpr bool is_valid_type = daw::traits::is_one_of_v<daw::traits::root_type_t<T>, daw::traits::root_type_t<Type>, daw::traits::root_type_t<Types>...>;
	private:
		std::array<uint8_t, BUFFER_SIZE> m_buffer;
		boost::optional<std::type_index> m_stored_type;
		std::function<std::string( )> m_to_string;

		template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, daw::traits::root_type_t<T>>>
		Result * ptr( ) {
			using namespace daw::exception;
			using value_type = daw::traits::root_type_t<T>;
			daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
			daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ), "Attempt to access a value of another type" );
			static_assert( sizeof(value_type) <= BUFFER_SIZE, "This should never happen.  sizeof(T) does not fit into m_buffer" );
			return reinterpret_cast<value_type *>(m_buffer.data( ));
		}

		template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, daw::traits::root_type_t<T>>>
		Result const * ptr( ) const {
			using namespace daw::exception;
			using value_type = daw::traits::root_type_t<T>;
			daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
			daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ), "Attempt to access a value of another type" );
			static_assert( sizeof(value_type) <= BUFFER_SIZE, "This should never happen.  sizeof(T) does not fit into m_buffer" );
			return reinterpret_cast<value_type const *>(m_buffer.data( ));	
		}

		template<typename T>
		static auto get_type_index( ) {
			using value_type = daw::traits::root_type_t<T>;
			return std::type_index( typeid( value_type ) );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		void set_type( ) {
			m_stored_type = get_type_index<T>( );
			m_to_string = [&]( ) {
				using std::to_string;
				return to_string( *ptr<T>( ) );
			};
		}

	public:
		variant_t( ):
				m_buffer{ },
				m_stored_type{ } { }

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t( T const & value ):
				variant_t{ } {

			store( value );
		}

		~variant_t( ) = default;
		variant_t( variant_t const & ) = default;
		variant_t & operator=( variant_t const & ) = default;
		variant_t( variant_t && ) = default;
		variant_t & operator=( variant_t && ) = default;

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t & operator=( T const & value ) {
			store( value );
			return *this;
		}

		friend void swap( variant_t & lhs, variant_t & rhs ) noexcept {
			lhs.m_buffer.swap( rhs.m_buffer );
			lhs.m_stored_type.swap( rhs.m_stored_type );
		}

		auto type_index( ) const {
			return m_stored_type;
		}

		bool empty( ) const {
			return !static_cast<bool>( m_stored_type );
		}

		explicit operator bool( ) const {
			return static_cast<bool>( m_stored_type );
		}

		void reset( ) {
			m_stored_type = boost::optional<std::type_index>{ };
			m_to_string = nullptr;
		}

		template<typename T>
		bool is_same_type( ) const {
			return m_stored_type == get_type_index<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t & store( T const & value ) {
			if( !is_same_type<T>() ) {
				set_type<T>( );
			}
			get<T>( ) = value;
			return *this;
		}

		auto compare( variant_t const & rhs ) const {
			if( this->equal( rhs ) ) {
				return 0;
			}
			return this->m_to_string( ).compare( rhs.m_to_string( ) );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		auto compare( T const & rhs ) const {
			return get<T>( ) - rhs;	
		}

		bool equal( variant_t const & rhs ) const {
			return std::tie( this->m_stored_type, this->m_buffer ) == std::tie( rhs.m_stored_type, rhs.m_buffer );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		auto eqaul( T const & rhs ) const {
			return this->get<T>( ) == rhs;	
		}

		friend bool operator==( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) == 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator==( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) == 0;
		}

		friend bool operator!=( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) != 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator!=( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) != 0;
		}

		friend bool operator<( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) < 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator<( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) < 0;
		}

		friend bool operator>( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) > 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator>( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) > 0;
		}

		friend bool operator<=( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) <= 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator<=( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) <= 0;
		}

		friend bool operator>=( variant_t const & lhs, variant_t const & rhs ) {
			return lhs.compare( rhs ) >= 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator>=( variant_t const & lhs, T const & rhs ) {
			return lhs.compare( rhs ) >= 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		T & get( ) {
			return *ptr<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		T const & get( ) const {
			return *ptr<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		explicit operator T const &( ) const {
			return get<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		explicit operator T &( ) {
			return get<T>( );
		}

		template<typename... Args>
		friend std::string to_string( variant_t<Args...> const & );
	};	// variant_t

	template<typename... Args>
	std::string to_string( variant_t<Args...> const & value ) {
		return value.m_to_string( );
	}

	template<typename... Args>
	std::ostream operator>>( std::ostream & os, variant_t<Args...> const & value ) {
		using std::to_string;
		os << to_string( value );
		return os;
	}

	template<typename Type, typename... Types>
	auto as_variant_t( Type const & value ) {
		return variant_t<Type, Types...>{ }.store( value );
	};

}	// namespace daw


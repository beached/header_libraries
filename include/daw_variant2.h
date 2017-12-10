// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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
#include <functional>
#include <typeindex>

#include "daw_static_array.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace variant_impl {
		template<typename T, typename... Ts>
		constexpr bool is_valid_v = daw::is_detected_v<daw::pack_index_of, T, Ts...>;

		template<typename... Ts>
		struct value_storage_t {
			static constexpr size_t const s_buffer_size = daw::traits::max_sizeof_v<Ts...>;
			alignas( daw::traits::max_sizeof_t<Ts...> ) daw::static_array_t<char, s_buffer_size> m_data;

			constexpr value_storage_t( ) noexcept
			  : m_data{0} {}

			~value_storage_t( ) noexcept = default;
			constexpr value_storage_t( value_storage_t const & ) noexcept = default;
			constexpr value_storage_t( value_storage_t && ) noexcept = default;
			constexpr value_storage_t &operator=( value_storage_t const & ) noexcept = default;
			constexpr value_storage_t &operator=( value_storage_t && ) noexcept = default;

			static constexpr size_t size( ) noexcept {
				return s_buffer_size;
			}

			template<typename T, typename... Args>
			void emplace( Args &&... args ) noexcept( noexcept( new( static_cast<void *>( m_data.data( ) ) )
			                                                      std::decay_t<T>{std::forward<Args>( args )...} ) ) {
				using base_t = std::decay_t<T>;
				static_assert( is_valid_v<base_t, Ts...>, "T must exist in stored types" );
				void *p = m_data.data( );
				new( p ) base_t{std::forward<Args>( args )...};
			}

			template<typename T>
			void construct( T &&value ) noexcept(
			  noexcept( std::declval<value_storage_t>( ).template emplace<T>( std::forward<T>( value ) ) ) ) {

				emplace<T>( std::forward<T>( value ) );
			}

			template<typename T, daw::required<is_valid_v<std::decay_t<T>, Ts...>> = nullptr>
			value_storage_t( T &&value ) noexcept(
			  noexcept( std::declval<value_storage_t>( ).construct( std::forward<T>( value ) ) ) &&
			  noexcept( value_storage_t{} ) )
			  : value_storage_t{} {

				construct( std::forward<T>( value ) );
			}

			template<typename T>
			void destruct( ) noexcept {
				static_assert( is_valid_v<T, Ts...>, "T must exist in stored types" );
				auto &tmp = ref<T>( );
				tmp.~T( );
			}

			template<typename T>
			T const *ptr( ) const noexcept {
				static_assert( is_valid_v<T, Ts...>, "T must exist in stored types" );
				return reinterpret_cast<T const *>( static_cast<void const *>( m_data.data( ) ) );
			}

			template<typename T>
			T *ptr( ) noexcept {
				static_assert( is_valid_v<T, Ts...>, "T must exist in stored types" );
				return reinterpret_cast<T *>( static_cast<void *>( m_data.data( ) ) );
			}

			template<typename T>
			T const &ref( ) const noexcept {
				static_assert( is_valid_v<T, Ts...>, "T must exist in stored types" );
				return *ptr<T>( );
			}

			template<typename T>
			T &ref( ) noexcept {
				static_assert( is_valid_v<T, Ts...>, "T must exist in stored types" );
				return *ptr<T>( );
			}
		}; // namespace variant_impl

		template<typename... Ts>
		struct variant_value_ops_t {
			std::function<void( value_storage_t<Ts...> & )> destruct;
			std::function<std::type_index( )> type_index;
		};
	} // namespace variant_impl

	// **************************************
	struct empty_variant {};
	constexpr bool operator==( empty_variant, empty_variant ) noexcept {
		return true;
	}
	constexpr bool operator!=( empty_variant, empty_variant ) noexcept {
		return false;
	}
	constexpr bool operator>( empty_variant, empty_variant ) noexcept {
		return false;
	}
	constexpr bool operator>=( empty_variant, empty_variant ) noexcept {
		return true;
	}
	constexpr bool operator<( empty_variant, empty_variant ) noexcept {
		return false;
	}
	constexpr bool operator<=( empty_variant, empty_variant ) noexcept {
		return true;
	}

	template<typename... Ts>
	struct variant {
		using index_t = uint8_t;
		static_assert( sizeof...( Ts ) < std::numeric_limits<index_t>::max( ),
		               "Numeric of types in variant exceeds maximum allowed" );

		static_assert( daw::traits::are_unique_v<Ts...>, "All types must be unique" );

		static_assert( daw::traits::isnt_cv_ref_v<Ts...>, "Const, reference, or volatile types are unsupported" );

	private:
		variant_impl::value_storage_t<Ts..., empty_variant> m_data;
		index_t m_index;

		static auto &get_value_ops( index_t idx ) noexcept {
			static daw::static_array_t<variant_impl::variant_value_ops_t<Ts..., empty_variant>, sizeof...( Ts ) + 1>
			  s_value_ops{};

			return s_value_ops[idx];
		}

		template<typename T>
		static constexpr size_t const index_of = daw::pack_index_of_v<T, Ts...>;

	public:
		constexpr variant( ) noexcept
		  : m_data{}
		  , m_index{sizeof...( Ts )} {}

		~variant( ) noexcept {
			get_value_ops( m_index ).destruct( m_data );
		}

		constexpr variant( variant const & ) = default;
		constexpr variant( variant && ) noexcept = default;
		constexpr variant &operator=( variant const & ) = default;
		constexpr variant &operator=( variant && ) noexcept = default;

		template<typename T, daw::required<variant_impl::is_valid_v<std::decay_t<T>, Ts...>> = nullptr>
		variant( T &&value )
		  : m_data{std::forward<T>( value )}
		  , m_index{index_of<T>} {}

		void reset( ) noexcept {
			if( index( ) != sizeof...( Ts ) ) {
				get_value_ops( m_index ).destruct( m_data );
			}
			m_index = sizeof...( Ts );
		}

		template<typename T, typename... Args, daw::required<variant_impl::is_valid_v<std::decay_t<T>, Ts...>> = nullptr>
		void emplace( Args &&... args ) {
			using base_t = std::decay_t<T>;
			if( index( ) != index_of<base_t> ) {
				reset( );
			}
			m_data.template emplace<base_t>( std::forward<Args>( args )... );
			m_index = index_of<base_t>;
		}

		template<typename T, daw::required<variant_impl::is_valid_v<std::decay_t<T>, Ts...>> = nullptr>
		variant &operator=( T &&value ) {
			emplace<T>( std::forward<T>( value ) );
			return *this;
		}

		constexpr bool empty( ) const noexcept {
			return m_index == sizeof...( Ts );
		}

		constexpr index_t index( ) const noexcept {
			return m_index;
		}

		std::type_index type_index( ) const {
			return get_value_ops( m_index ).type_index( );
		}

		template<typename T>
		T const &get( ) const {
			static_assert( variant_impl::is_valid_v<std::decay_t<T>, Ts...>, "T must exist in stored types" );
			daw::exception::daw_throw_on_false( index_of<T> == index( ), "Attempt to get a mismatched type" );

			return m_data.template ref<T>( );
		}

		template<typename T>
		T &get( ) {
			static_assert( variant_impl::is_valid_v<std::decay_t<T>, Ts...>, "T must exist in stored types" );
			daw::exception::daw_throw_on_false( index_of<T> == index( ), "Attempt to get a mismatched type" );

			return m_data.template ref<T>( );
		}

		template<typename T>
		operator T const &( ) const {
			static_assert( variant_impl::is_valid_v<std::decay_t<T>, Ts...>, "T must exist in stored types" );
			return get<T>( );
		}

		template<typename T>
		operator T &( ) {
			static_assert( variant_impl::is_valid_v<std::decay_t<T>, Ts...>, "T must exist in stored types" );
			return get<T>( );
		}
	};

} // namespace daw

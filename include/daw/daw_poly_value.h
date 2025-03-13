// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_enable_if.h"
#include "daw_traits.h"

#include <functional>
#include <memory>
#include <utility>

namespace daw {
	namespace poly_value_impl {
		template<typename Base>
		struct poly_value_storage_base {
		protected:
			poly_value_storage_base( ) = default;

		public:
			virtual ~poly_value_storage_base( ) = default;
			virtual constexpr std::unique_ptr<poly_value_storage_base>
			copy( ) const = 0;
			virtual constexpr std::unique_ptr<poly_value_storage_base>
			move( ) noexcept = 0;
			virtual constexpr Base const *get( ) const = 0;
			virtual constexpr Base *get( ) = 0;

			poly_value_storage_base( poly_value_storage_base const & ) = delete;

			poly_value_storage_base( poly_value_storage_base && ) = delete;

			poly_value_storage_base &
			operator=( poly_value_storage_base const & ) = delete;

			poly_value_storage_base &operator=( poly_value_storage_base && ) = delete;
		};

		template<typename Base, typename T>
		class poly_value_storage : public poly_value_storage_base<Base> {
			DAW_NO_UNIQUE_ADDRESS T m_value;

		public:
			template<typename... Args>
			requires( not traits::is_first_type_v<poly_value_storage, Args...> ) //
			  constexpr poly_value_storage( Args &&...args )
			  : m_value( DAW_FWD( args )... ) {}

			constexpr std::unique_ptr<poly_value_storage_base<Base>>
			copy( ) const override {
				return std::unique_ptr<poly_value_storage_base<Base>>(
				  new poly_value_storage( m_value ) );
			}

			constexpr std::unique_ptr<poly_value_storage_base<Base>>
			move( ) noexcept override {
				return std::unique_ptr<poly_value_storage_base<Base>>(
				  new poly_value_storage( std::move( m_value ) ) );
			}

			constexpr Base const *get( ) const override {
				return std::addressof( m_value );
			}

			constexpr Base *get( ) override {
				return std::addressof( m_value );
			}
		};

		template<typename Base, typename T = Base, typename... Args>
		requires std::is_base_of_v<Base, T> //
		  constexpr std::unique_ptr<poly_value_storage_base<Base>>
		  create( Args &&...args ) {
			return std::unique_ptr<poly_value_storage_base<Base>>(
			  new poly_value_storage<Base, T>( DAW_FWD( args )... ) );
		}
	} // namespace poly_value_impl

	template<typename T = void>
	struct construct_emplace_t {};

	template<typename T = void>
	inline constexpr construct_emplace_t<T> construct_emplace{ };

	template<typename Base>
	class poly_value {
		std::unique_ptr<poly_value_impl::poly_value_storage_base<Base>>
		  m_storage{ };

	public:
		constexpr poly_value( ) requires( std::is_default_constructible_v<Base> )
		  : m_storage( poly_value_impl::create<Base>( ) ) {}

		template<typename T>
		requires std::is_base_of_v<Base, daw::remove_cvref_t<T>> //
		  constexpr poly_value( T &&value )
		  : m_storage( poly_value_impl::create<Base, std::remove_cvref_t<T>>(
		      DAW_FWD( value ) ) ) {}

		template<typename T, typename... Args>
		requires std::is_base_of_v<Base, daw::remove_cvref_t<T>> //
		  constexpr poly_value( construct_emplace_t<T>, Args &&...args )
		  : m_storage( poly_value_impl::create<Base, std::remove_cvref_t<T>>(
		      DAW_FWD( args )... ) ) {}

		constexpr poly_value( poly_value const &other )
		  : m_storage( other.m_storage->copy( ) ) {}

		constexpr poly_value &operator=( poly_value const &rhs ) {
			if( this != &rhs ) {
				m_storage = rhs.m_storage->copy( );
			}
			return *this;
		}

		constexpr poly_value( poly_value &&other ) noexcept
		  : m_storage( other.m_storage->move( ) ) {}

		constexpr poly_value &operator=( poly_value &&rhs ) noexcept {
			if( this != &rhs ) {
				m_storage = rhs.m_storage->move( );
			}
			return *this;
		}

		~poly_value( ) = default;

		template<typename Child>
		requires( std::is_base_of_v<Base, Child> and
		          not std::is_same_v<Base, Child> ) //
		  poly_value( poly_value<Child> const &other )
		  : m_storage( other.m_storage->copy( ) ) {}

		template<typename Child>
		requires( std::is_base_of_v<Base, Child> and
		          not std::is_same_v<Base, Child> ) //
		  poly_value( poly_value<Child> &&other )
		  : m_storage( other.m_storage->move( ) ) {}

		Base const *get( ) const {
			return m_storage->get( );
		}

		Base *get( ) {
			return m_storage->get( );
		}

		Base const &operator*( ) const {
			return *get( );
		}

		Base &operator*( ) {
			return *get( );
		}

		Base const *operator->( ) const {
			return get( );
		}

		Base *operator->( ) {
			return get( );
		}

		operator Base &( ) & noexcept {
			return *get( );
		}

		operator Base const &( ) const & noexcept {
			return *get( );
		}

		operator Base &&( ) && noexcept {
			return std::move( *get( ) );
		}

		operator Base &&( ) const && noexcept {
			return std::move( *get( ) );
		}
	};
} // namespace daw

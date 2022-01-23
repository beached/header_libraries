// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_traits.h"
#include "daw_move.h"

#include <ciso646>
#include <functional>
#include <optional>
#include <type_traits>

namespace daw {
	inline constexpr std::nullopt_t nullopt = std::nullopt;

	template<typename T, typename U = void>
	struct optional;

	template<typename T>
	struct optional<T, std::enable_if_t<not std::is_reference_v<T>>> {
		using value_type = T;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using rvalue_reference = std::add_rvalue_reference_t<value_type>;
		using const_rvalue_reference =
		  std::add_rvalue_reference_t<std::add_const_t<value_type>>;
		using pointer = std::add_pointer_t<value_type>;
		using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;

	private:
		std::optional<value_type> m_value = std::optional<value_type>( );

	public:
		constexpr optional( ) noexcept = default;
		constexpr optional( std::nullopt_t ) noexcept {}

		[[nodiscard]] constexpr reference operator*( ) &noexcept {
			return m_value.operator*( );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const &noexcept {
			return m_value.operator*( );
		}

		[[nodiscard]] constexpr rvalue_reference operator*( ) &&noexcept {
			return DAW_MOVE( m_value ).operator*( );
		}

		[[nodiscard]] constexpr const_rvalue_reference
		operator*( ) const &&noexcept {
			return DAW_MOVE( m_value ).operator*( );
		}

		[[nodiscard]] constexpr pointer operator->( ) noexcept {
			return &m_value.operator->( );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const noexcept {
			return &m_value.operator->( );
		}

		void swap( optional &other ) noexcept(
		  std::is_nothrow_swappable_v<std::optional<value_type>> ) {
			using std::swap;
			swap( m_value, other.m_value );
		}

		void reset( ) noexcept {
			m_value.reset( );
		}

		template<typename... Args>
		reference emplace( Args &&...args ) {
			return m_value.emplace( std::forward<Args>( args )... );
		}

		// cannot emplace to a reference
		template<typename U, typename... Args>
		reference emplace( std::initializer_list<U> ilist, Args &&...args ) {
			return m_value.emplace( DAW_MOVE( ilist ),
			                        std::forward<Args>( args )... );
		}

		[[nodiscard]] constexpr reference value( ) & {
			return m_value.value( );
		}

		[[nodiscard]] constexpr const_reference value( ) const & {
			return m_value.value( );
		}

		[[nodiscard]] constexpr rvalue_reference value( ) && {
			return DAW_MOVE( m_value ).value( );
		}

		[[nodiscard]] constexpr const_rvalue_reference value( ) const && {
			return DAW_MOVE( m_value ).value( );
		}

		[[nodiscard]] constexpr bool has_value( ) const noexcept {
			return m_value.has_value( );
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_value.has_value( );
		}

		template<typename U = value_type,
		         std::enable_if_t<
		           daw::all_true_v<
		             std::is_constructible_v<std::optional<T>, U>,
		             std::is_convertible_v<daw::remove_cvref_t<U> const &, T>,
		             not std::is_same_v<daw::remove_cvref_t<U>, optional<T>>>,
		           std::nullptr_t> = nullptr>
		constexpr optional( U &&value )
		  : m_value( std::forward<U>( value ) ) {}

		template<typename U = value_type,
		         std::enable_if_t<
		           daw::all_true_v<
		             std::is_constructible_v<std::optional<T>, U>,
		             not std::is_convertible_v<daw::remove_cvref_t<U> const &, T>,
		             not std::is_same_v<daw::remove_cvref_t<U>, optional<T>>>,
		           std::nullptr_t> = nullptr>
		constexpr explicit optional( U &&value )
		  : m_value( std::forward<U>( value ) ) {}

		optional &operator=( std::nullopt_t ) noexcept {
			m_value = nullopt;
			return *this;
		}

		template<
		  typename U = T,
		  std::enable_if_t<not std::is_same_v<daw::remove_cvref_t<U>, optional<T>>,
		                   std::nullptr_t> = nullptr>
		optional &operator=( U &&value ) {
			m_value = std::forward<U>( value );
			return *this;
		}
	};

	template<typename T>
	struct optional<T, std::enable_if_t<std::is_lvalue_reference_v<T>>> {
		using value_type = std::remove_reference_t<T>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using rvalue_reference = std::add_rvalue_reference_t<value_type>;
		using const_rvalue_reference =
		  std::add_rvalue_reference_t<std::add_const_t<value_type>>;
		using pointer = std::add_pointer_t<value_type>;
		using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;

	private:
		std::optional<value_type *> m_value = std::optional<value_type *>( );

	public:
		constexpr optional( ) noexcept = default;
		constexpr optional( std::nullopt_t ) noexcept {}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			return *m_value.operator*( );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			return *m_value.operator*( );
		}

		[[nodiscard]] constexpr pointer operator->( ) noexcept {
			return m_value.operator*( );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const noexcept {
			return m_value.operator*( );
		}

		void swap( optional &other ) noexcept {
			using std::swap;
			swap( m_value, other.m_value );
		}

		void reset( ) noexcept {
			m_value.reset( );
		}

		// cannot emplace to a reference
		template<typename... Args>
		reference emplace( Args &&... ) = delete;

		// cannot emplace to a reference
		template<typename U, typename... Args>
		reference emplace( std::initializer_list<U>, Args &&... ) = delete;

		[[nodiscard]] constexpr reference value( ) {
			return *m_value.value( );
		}

		[[nodiscard]] constexpr const_reference value( ) const {
			return *m_value.value( );
		}

		[[nodiscard]] constexpr bool has_value( ) const noexcept {
			return m_value.has_value( );
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_value.has_value( );
		}

		template<typename U = value_type,
		         std::enable_if_t<
		           daw::all_true_v<
		             std::is_constructible_v<pointer, daw::remove_cvref_t<U> *>,
		             not std::is_rvalue_reference_v<U>,
		             not std::is_same_v<daw::remove_cvref_t<U>, optional<T>>>,
		           std::nullptr_t> = nullptr>
		constexpr explicit optional( U &&value )
		  : m_value( &value ) {}

		optional &operator=( std::nullopt_t ) noexcept {
			m_value = nullopt;
			return *this;
		}

		/*
		template<typename U = T,
		         std::enable_if_t<daw::all_true_v<not std::is_same_v<
		                              daw::remove_cvref_t<U>, optional<T>>>,
		                            std::nullptr_t> = nullptr>
		optional &operator=( U &&value ) {
		  if( not m_value ) {
		    m_value = &value;
		  } else {
		    if constexpr( not std::is_const_v<value_type> ) {
		      **m_value = std::forward<U>( value );
		    } else {
		      std::abort( );
		    }
		  }
		  return *this;
		}*/
	};

	// logical operators
	template<typename T, typename U>
	constexpr bool operator==( optional<T> const &lhs, optional<U> const &rhs ) {
		if( lhs.has_value( ) != rhs.has_value( ) ) {
			return false;
		}
		if( not lhs ) {
			return true;
		}
		return *lhs == *rhs;
	}

	template<typename T, typename U>
	constexpr bool operator!=( optional<T> const &lhs, optional<U> const &rhs ) {
		if( lhs.has_value( ) != rhs.has_value( ) ) {
			return true;
		}
		if( not lhs ) {
			return false;
		}
		return *lhs != *rhs;
	}

	template<typename T, typename U>
	constexpr bool operator<( optional<T> const &lhs, optional<U> const &rhs ) {
		if( not rhs ) {
			return false;
		}
		if( not lhs ) {
			return true;
		}
		return *lhs < *rhs;
	}

	template<typename T, typename U>
	constexpr bool operator<=( optional<T> const &lhs, optional<U> const &rhs ) {
		if( not lhs ) {
			return true;
		}
		if( not rhs ) {
			return false;
		}
		return *lhs <= *rhs;
	}

	template<typename T, typename U>
	constexpr bool operator>( optional<T> const &lhs, optional<U> const &rhs ) {
		if( not lhs ) {
			return false;
		}
		if( not rhs ) {
			return true;
		}
		return *lhs > *rhs;
	}

	template<typename T, typename U>
	constexpr bool operator>=( optional<T> const &lhs, optional<U> const &rhs ) {
		if( not rhs ) {
			return true;
		}
		if( not lhs ) {
			return false;
		}
		return *lhs >= *rhs;
	}

	template<typename T>
	constexpr bool operator==( optional<T> const &opt, std::nullopt_t ) noexcept {
		return not opt;
	}

	template<typename T>
	constexpr bool operator==( std::nullopt_t, optional<T> const &opt ) noexcept {
		return not opt;
	}

	template<typename T>
	constexpr bool operator!=( optional<T> const &opt, std::nullopt_t ) noexcept {
		return static_cast<bool>( opt );
	}

	template<typename T>
	constexpr bool operator!=( std::nullopt_t, optional<T> const &opt ) noexcept {
		return static_cast<bool>( opt );
	}

	template<typename T>
	constexpr bool operator<( optional<T> const &, std::nullopt_t ) noexcept {
		return false;
	}

	template<typename T>
	constexpr bool operator<( std::nullopt_t, optional<T> const &opt ) noexcept {
		return static_cast<bool>( opt );
	}

	template<typename T>
	constexpr bool operator<=( optional<T> const &opt, std::nullopt_t ) noexcept {
		return not opt;
	}

	template<typename T>
	constexpr bool operator<=( std::nullopt_t,
	                           optional<T> const & /*opt*/ ) noexcept {
		return true;
	}

	template<typename T>
	constexpr bool operator>( optional<T> const &opt, std::nullopt_t ) noexcept {
		return static_cast<bool>( opt );
	}

	template<typename T>
	constexpr bool operator>( std::nullopt_t,
	                          optional<T> const & /*opt*/ ) noexcept {
		return false;
	}

	template<typename T>
	constexpr bool operator>=( optional<T> const & /*opt*/,
	                           std::nullopt_t ) noexcept {
		return true;
	}

	template<typename T>
	constexpr bool operator>=( std::nullopt_t, optional<T> const &opt ) noexcept {
		return static_cast<bool>( opt );
	}

	template<typename T, typename U>
	constexpr bool operator==( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt == value;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator==( T const &value, optional<U> const &opt ) {
		if( opt ) {
			return value == *opt;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator!=( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt != value;
		}
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator!=( T const &value, optional<U> const &opt ) {
		if( opt ) {
			return value != *opt;
		}
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator<( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt < value;
		}
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator<( T const &value, optional<U> const &opt ) {
		if( opt ) {
			return value < *opt;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator<=( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt <= value;
		}
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator<=( T const &value, optional<U> const &opt ) {
		if( opt ) {
			return value <= *opt;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator>( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt > value;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator>( T const &value, optional<U> const &opt ) {
		if( opt ) {
			return value > *opt;
		}
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator>=( optional<T> const &opt, U const &value ) {
		if( opt ) {
			return *opt >= value;
		}
		return false;
	}

	template<typename T, typename U>
	constexpr bool operator>=( const T &value, const optional<U> &opt ) {
		if( opt ) {
			return *opt > value;
		}
		return true;
	}
} // namespace daw

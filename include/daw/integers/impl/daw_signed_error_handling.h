// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_arith_traits.h"
#include "daw/daw_attributes.h"
#include "daw/daw_check_exceptions.h"
#include "daw/daw_cpp_feature_check.h"

#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

/// \brief DAW_DEFAULT_SIGNED_CHECKING can be defined to the following
/// 0 - Checked with wrapping defaults if not a named op(e.g. add_wrapped)(for
/// add/sub/mul) 1 - Unchecked 2 - Wrapped for add/sub/mul, uncheckd for others
#if not defined( DAW_DEFAULT_SIGNED_CHECKING )
#if defined( DEBUG ) or not defined( NDEBUG )
#define DAW_DEFAULT_SIGNED_CHECKING 0
#else
#define DAW_DEFAULT_SIGNED_CHECKING 1
#endif
#endif

namespace daw::integers {
	enum class SignedIntegerErrorType { Overflow, DivideByZero };
	using signed_int_error_handler_t = void ( * )( void *,
	                                               SignedIntegerErrorType );

	namespace sint_impl {
		inline auto &get_signed_integer_overflow_handler( ) {
			static DAW_CONSTINIT struct handler_t {
				signed_int_error_handler_t cb = nullptr;
				void *data = nullptr;
			} handler{ };
			return handler;
		}

		inline auto &get_signed_integer_div_by_zero_handler( ) {
			static DAW_CONSTINIT struct handler_t {
				signed_int_error_handler_t cb = nullptr;
				void *data = nullptr;
			} handler{ };
			return handler;
		}
	} // namespace sint_impl

	/// Caller is responsible for ensuring that this is called in a context that
	/// protects against multiple threads accessing/writing at the same time
	DAW_ATTRIB_NOINLINE inline void register_signed_overflow_handler(
	  signed_int_error_handler_t handler = nullptr,
	  void *data = nullptr ) noexcept {
		sint_impl::get_signed_integer_overflow_handler( ).cb = handler;
		sint_impl::get_signed_integer_overflow_handler( ).data = data;
	}

	/// Caller is responsible for ensuring that this is called in a context that
	/// protects against multiple threads accessing/writing at the same time
	template<typename Func,
	         std::enable_if_t<std::is_class_v<Func> and
	                            std::is_invocable_v<Func, SignedIntegerErrorType>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_NOINLINE inline void
	register_signed_overflow_handler( Func &handler ) noexcept {
		if constexpr( std::is_const_v<Func> ) {
			register_signed_overflow_handler(
			  +[]( void *vhnd, SignedIntegerErrorType error_type ) {
				  (void)( *static_cast<Func const *>( vhnd ) )( error_type );
			  },
			  const_cast<void *>(
			    static_cast<void const *>( std::addressof( handler ) ) ) );
		} else {
			register_signed_overflow_handler(
			  +[]( void *vhnd, SignedIntegerErrorType error_type ) {
				  (void)( *static_cast<Func *>( vhnd ) )( error_type );
			  },
			  static_cast<void *>( std::addressof( handler ) ) );
		}
	}

	/// Caller is responsible for ensuring that this is called in a context that
	/// protects against multiple threads accessing/writing at the same time
	DAW_ATTRIB_NOINLINE inline void register_signed_div_by_zero_handler(
	  signed_int_error_handler_t handler = nullptr,
	  void *data = nullptr ) noexcept {
		sint_impl::get_signed_integer_div_by_zero_handler( ).cb = handler;
		sint_impl::get_signed_integer_div_by_zero_handler( ).data = data;
	}

	/// Caller is responsible for ensuring that this is called in a context that
	/// protects against multiple threads accessing/writing at the same time
	template<typename Func,
	         std::enable_if_t<std::is_class_v<Func> and
	                            std::is_invocable_v<Func, SignedIntegerErrorType>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_NOINLINE inline void
	register_signed_div_by_zero_handler( Func &handler ) noexcept {
		if constexpr( std::is_const_v<Func> ) {
			register_signed_div_by_zero_handler(
			  +[]( void *vhnd, SignedIntegerErrorType error_type ) {
				  (void)( *static_cast<Func const *>( vhnd ) )( error_type );
			  },
			  const_cast<void *>(
			    static_cast<void const *>( std::addressof( handler ) ) ) );
		} else {
			register_signed_div_by_zero_handler(
			  +[]( void *vhnd, SignedIntegerErrorType error_type ) {
				  (void)( *static_cast<Func *>( vhnd ) )( error_type );
			  },
			  static_cast<void *>( std::addressof( handler ) ) );
		}
	}

	struct signed_integer_overflow_exception : std::exception {};
	struct signed_integer_div_by_zero_exception : std::exception {};

	DAW_ATTRIB_NOINLINE inline void on_signed_integer_overflow( ) {
		auto handler = sint_impl::get_signed_integer_overflow_handler( );
		if( handler.cb ) {
			handler.cb( handler.data, SignedIntegerErrorType::Overflow );
			return;
		}
		DAW_THROW_OR_TERMINATE_NA( signed_integer_overflow_exception );
	}

	DAW_ATTRIB_NOINLINE inline void on_signed_integer_div_by_zero( ) {
		auto handler = sint_impl::get_signed_integer_div_by_zero_handler( );
		if( handler.cb ) {
			handler.cb( handler.data, SignedIntegerErrorType::DivideByZero );
			return;
		}
		DAW_THROW_OR_TERMINATE_NA( signed_integer_div_by_zero_exception );
	}
} // namespace daw::integers

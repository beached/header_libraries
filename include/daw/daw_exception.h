// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_attributes.h"
#include "daw_move.h"
#include "daw_unused.h"

#include <cstdlib>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <type_traits>

#if defined( DAW_USE_EXCEPTIONS ) or not defined( NODEBUGTHROW )

#define MAY_THROW_EXCEPTIONS true
#include <stdexcept>

#else

#define MAY_THROW_EXCEPTIONS false

#endif

namespace daw::exception {
	struct basic_exception {
		basic_exception( ) = default;
	};
	struct arithmetic_exception : public basic_exception {};
	struct not_implemented_exception : public basic_exception {};

	[[maybe_unused]] constexpr bool may_throw_v = MAY_THROW_EXCEPTIONS;

#if MAY_THROW_EXCEPTIONS
	using NotImplemented = std::runtime_error;
	using FatalError = std::runtime_error;
	using NullPtrAccessException = std::runtime_error;
	using AssertException = std::runtime_error;
	using FileException = std::runtime_error;
	using MethodNotImplemented = std::runtime_error;
	using UnexpectedEnumValue = std::runtime_error;
	using DefaultException = std::runtime_error;
#else
	using NotImplemented = basic_exception;
	using FatalError = basic_exception;
	using NullPtrAccessException = basic_exception;
	using AssertException = basic_exception;
	using FileException = basic_exception;
	using MethodNotImplemented = basic_exception;
	using UnexpectedEnumValue = basic_exception;
	using DefaultException = basic_exception;
#endif

	template<typename T, T error_number>
	struct errno_exception : basic_exception,
	                         std::integral_constant<T, error_number> {};

	template<typename ExceptionType = DefaultException,
	         std::enable_if_t<std::is_default_constructible_v<ExceptionType>,
	                          std::nullptr_t> = nullptr>
	[[noreturn]] DAW_ATTRIB_NOINLINE void daw_throw( ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		throw ExceptionType{ };
#else
		std::abort( );
#endif
	}

	template<typename ExceptionType = DefaultException,
	         std::enable_if_t<not std::is_default_constructible_v<ExceptionType>,
	                          std::nullptr_t> = nullptr>
	[[noreturn]] void daw_throw( ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		throw ExceptionType( "" );
#else
		std::abort( );
#endif
	}

	template<typename ExceptionType = DefaultException, typename Arg,
	         typename... Args>
	[[noreturn]] void daw_throw( Arg &&arg, Args &&...args ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		throw ExceptionType( DAW_FWD( arg ), DAW_FWD( args )... );
#else
		(void)arg;
		( (void)args, ... );
		std::abort( );
#endif
	}

#ifndef NODEBUGTHROW
	template<typename ExceptionType = DefaultException, typename... Args>
	[[noreturn]] constexpr void debug_throw( Args &&...args ) {
		daw_throw<ExceptionType>( DAW_FWD( args )... );
	}
#else
	template<typename ExceptionType = DefaultException, typename... Args>
	constexpr void debug_throw( Args &&... ) {}
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void dbg_throw_on_null( ValueType const *const value,
	                                  Args &&...args ) {
		if( nullptr == value ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}
#else
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void dbg_throw_on_null( ValueType &&, Args &&... ) {}

#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr ValueType *dbg_throw_on_null_or_return( ValueType *value,
	                                                  Args &&...args ) {
		if( nullptr == value ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return value;
	}
#else
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr ValueType *dbg_throw_on_null_or_return( ValueType *value,
	                                                  Args &&...args ) {
		return value;
	}
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr auto dbg_throw_on_false_or_return( ValueType &&value, bool test,
	                                             Args &&...args ) {
		if( not static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return DAW_FWD( value );
	}

	template<typename ExceptionType = AssertException, typename... Args>
	constexpr bool dbg_throw_on_false_or_return( bool test, Args &&...args ) {
		if( not static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return true;
	}
#else
#define dbg_throw_on_false_or_return( test, ... ) ( test )
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_throw_on_false( Bool &&test, Args &&...args ) {
		if( not static_cast<bool>( DAW_FWD( test ) ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void DebugAssert( Bool &&test, Args &&...args ) {
		if( not static_cast<bool>( DAW_FWD( test ) ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException>
	constexpr void dbg_throw_on_false( bool test ) {
		if( not static_cast<bool>( test ) ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
			throw ExceptionType{ };
#else
			std::abort( );
#endif
		}
	}
#else
	template<typename... T>
	constexpr void dbg_throw_on_false( T... ) noexcept {}

	template<typename... T>
	constexpr void DebugAssert( T... ) noexcept {}
#endif

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType &dbg_throw_on_true_or_return( ValueType &value, bool test,
	                                                  Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return value;
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr bool dbg_throw_on_true_or_return( const Bool &test,
	                                            Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return test;
	}

	template<typename ExceptionType = AssertException, typename... Args>
	constexpr void dbg_throw_on_true( bool test, Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void daw_throw_on_null( ValueType const *const value,
	                                  Args &&...args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr void daw_throw_on_null( ValueType *value, Args &&...args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType *daw_throw_on_null_or_return( ValueType *value,
	                                                  Args &&...args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return value;
	}

	template<typename ExceptionType = AssertException, typename... Args,
	         typename Bool>
	constexpr bool daw_throw_on_false_or_return( Bool &&test, Args &&...args ) {
		if( not static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return true;
	}

	struct Terminator {};

	template<typename ExceptionType = Terminator, typename Bool, typename... Args>
	constexpr void precondition_check( Bool &&condition, Args &&...args ) {
		if( not static_cast<bool>( condition ) ) {
			if constexpr( std::is_same_v<Terminator, ExceptionType> ) {
				Unused( args... );
				std::abort( );
			} else {
				daw_throw<ExceptionType>( DAW_FWD( args )... );
			}
		}
	}

	template<typename ExceptionType = Terminator, typename Bool, typename... Args>
	constexpr void postcondition_check( Bool &&condition, Args &&...args ) {
		if( not static_cast<bool>( condition ) ) {
			if constexpr( std::is_same_v<Terminator, ExceptionType> ) {
				std::abort( );
			} else {
				daw_throw<ExceptionType>( DAW_FWD( args )... );
			}
		}
	}
#ifndef NODEBUGTHROW
	template<typename Exception = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_precondition_check( Bool &&condition, Args &&... ) {

		//			static_assert( std::is_convertible_v<Bool, bool> );
		if( not static_cast<bool>( condition ) ) {
			std::abort( );
		}
	}

	template<typename Bool, typename... Args>
	constexpr void dbg_postcondition_check( Bool &&condition, Args &&... ) {
		static_assert( std::is_convertible_v<Bool, bool> );
		if( not static_cast<bool>( condition ) ) {
			std::abort( );
		}
	}
#else
	template<typename Exception = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_precondition_check( Bool &&, Args &&... ) {}

	template<typename Bool, typename... Args>
	constexpr void dbg_postcondition_check( Bool &&, Args &&... ) {}
#endif
	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType daw_throw_on_false_or_return( ValueType &&value,
	                                                  Bool &&test,
	                                                  Args &&...args ) {
		if( not static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return DAW_FWD( value );
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void daw_throw_on_false( Bool &&test, Args &&...args ) {
		if( not static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void Assert( Bool const &test, Args &&...args ) {
		if( not static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_false( Bool &&test ) {
		if( not static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_true( Bool const &test ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<typename ExceptionType = DefaultException>
	constexpr void daw_throw_value_on_true( ExceptionType &&test ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( test ) );
		}
	}

	template<typename ExceptionType>
	constexpr void daw_throw_value_on_false( ExceptionType &&test ) {
		if( not static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( test ) );
		}
	}

	template<typename ExceptionType = MethodNotImplemented>
	[[noreturn]] constexpr void daw_throw_not_implemented( ) {
		daw_throw<ExceptionType>( "Method not implemented" );
	}

	template<typename ExceptionType = UnexpectedEnumValue>
	[[noreturn]] constexpr void daw_throw_unexpected_enum( ) {
		daw_throw<ExceptionType>( "Unknown/Unexpected enum" );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType daw_throw_on_true_or_return( ValueType &&value,
	                                                 Bool &&test,
	                                                 Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return DAW_FWD( value );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType &&daw_throw_on_true_or_return( ValueType &&value,
	                                                   Bool &&test,
	                                                   Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return DAW_FWD( value );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType daw_throw_on_true_or_return( ValueType &&value, bool test,
	                                                 Args &&...args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return DAW_FWD( value );
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr bool daw_throw_on_true_or_return( Bool &&test, Args &&...args ) {
		if( static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
		return false;
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_true( Bool &&test ) {
		if( static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<
	  typename ExceptionType = AssertException, typename Bool, typename... Args,
	  std::enable_if_t<( sizeof...( Args ) > 0 ), std::nullptr_t> = nullptr>
	constexpr void daw_throw_on_true( Bool &&test, Args &&...args ) {
		if( static_cast<bool>( DAW_FWD( test ) ) ) {
			daw_throw<ExceptionType>( DAW_FWD( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Predicate,
	         typename Message, typename Container>
	constexpr void assert_all_false( Container &&container,
	                                 Message &&assert_message,
	                                 Predicate &&predicate ) {
		auto first = std::begin( container );
		auto last = std::end( container );
		while( first != last ) {
			if( predicate( *first ) ) {
				daw_throw<ExceptionType>( DAW_FWD( assert_message ) );
			}
			++first;
		}
	}

	template<typename ExceptionType = AssertException, typename Predicate,
	         typename Message, typename Container>
	constexpr void assert_all_true( Container const &container,
	                                Message &&assert_message,
	                                Predicate &&predicate ) {
		assert_all_false<ExceptionType>(
		  container, DAW_FWD( assert_message ),
		  [pred = DAW_FWD( predicate )]( auto const &v ) {
			  return not pred( v );
		  } );
	}

	template<typename Function, typename... Args>
	void no_exception( Function func, Args &&...args ) noexcept {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		try {
#endif
			func( DAW_FWD( args )... );
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
		} catch( ... ) {}
#endif
	}

#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
	template<typename Exception, typename... Args>
	std::exception_ptr make_exception_ptr( Args &&...args ) noexcept {
		try {
			throw Exception{ DAW_FWD( args )... };
		} catch( ... ) { return std::current_exception( ); }
	}
#endif

	template<typename... Args>
	[[noreturn]] void throw_length_error( Args &&...args ) {
		daw_throw<std::length_error>( DAW_FWD( args )... );
	}

	template<typename... Args>
	[[noreturn]] void throw_out_of_range( Args &&...args ) {
		daw_throw<std::out_of_range>( DAW_FWD( args )... );
	}
} // namespace daw::exception

#if defined( NDEBUG ) and not defined( DEBUG )
#define DAW_DBG_PRECONDITION_CHECK( ... ) \
	do {                                    \
	} while( false )
#else
#define DAW_DBG_PRECONDITION_CHECK( Except, ... ) \
	daw::exception::precondition_check<Except>( __VA_ARGS__ )
#endif

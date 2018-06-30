// The MIT License (MIT)
// w much of a difference
//
// Copyright (c) 2013-2018 Darrell Wright
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

#include <stdexcept>
#include <string>

#include "daw_string.h"

namespace daw {
	namespace exception {
		using NotImplemented = std::runtime_error;
		using FatalError = std::runtime_error;
		using NullPtrAccessException = std::runtime_error;
		using AssertException = std::runtime_error;
		using FileException = std::runtime_error;
		using MethodNotImplemented = std::runtime_error;
		using UnexpectedEnumValue = std::runtime_error;

		struct basic_exception {};
		struct arithmetic_exception : public basic_exception {};
		struct not_implemented_exception : public basic_exception {};

		template<typename T, T error_number>
		struct errno_exception : public basic_exception,
		                         public std::integral_constant<T, error_number> {};

		template<typename ExceptionType = std::runtime_error, typename StringType>
		[[noreturn]] constexpr void daw_throw( StringType const &msg ) {
			throw ExceptionType( msg );
		}

#ifndef NODEBUGTHROW
		template<typename ExceptionType = std::runtime_error, typename StringType>
		[[noreturn]] constexpr void debug_throw( StringType const &msg ) {
			daw_throw<ExceptionType>( msg );
		}
#else
#define debug_throw( msg ) ;
#endif

#ifndef NODEBUGTHROW
		template<typename ExceptionType = NullPtrAccessException,
		         typename ValueType, typename StringType>
		constexpr void dbg_throw_on_null( ValueType const *const value,
		                                  StringType const &msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
		}
#else
#define dbg_throw_on_null( value, msg ) ;
#endif

#ifndef NODEBUGTHROW
		template<typename ExceptionType = NullPtrAccessException,
		         typename ValueType, typename StringType>
		constexpr ValueType *dbg_throw_on_null_or_return( ValueType *value,
		                                                  StringType const &msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}
#else
#define dbg_throw_on_null_or_return( value, msg ) ( value );
#endif

#ifndef NODEBUGTHROW
		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType>
		constexpr auto dbg_throw_on_false_or_return( ValueType &&value, bool test,
		                                             StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename StringType>
		constexpr bool dbg_throw_on_false_or_return( bool test,
		                                             StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
			return true;
		}
#else
#define dbg_throw_on_false_or_return( test, ... ) ( test );
#endif

#ifndef NODEBUGTHROW
		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr void dbg_throw_on_false( Bool const &test,
		                                   StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr void DebugAssert( Bool const &test, StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType, typename... Args>
		void dbg_throw_on_false( Bool const &test, StringType &&format,
		                         Args &&... args ) {
			if( !static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
		}

		template<typename ExceptionType = AssertException>
		constexpr void dbg_throw_on_false( bool test ) {
			if( !static_cast<bool>( test ) ) {
				throw ExceptionType{};
			}
		}
#else
		template<typename... T>
		constexpr void dbg_throw_on_false( T... ) noexcept {};

		template<typename... T>
		constexpr void DebugAssert( T... ) noexcept {};
#endif

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType>
		constexpr ValueType &dbg_throw_on_true_or_return( ValueType &value,
		                                                  bool test,
		                                                  StringType const &msg ) {
			if( static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr bool dbg_throw_on_true_or_return( const Bool &test,
		                                            StringType const &msg ) {
			if( static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
			return test;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		constexpr void dbg_throw_on_true( bool test, StringType const &msg ) {
			if( static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType,
		         typename... Args>
		constexpr void dbg_throw_on_true( bool test, StringType &&format,
		                                  Args &&... args ) {
			if( static_cast<bool>( test ) ) {
				debug_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
		}

		template<typename ExceptionType = NullPtrAccessException,
		         typename ValueType, typename StringType>
		constexpr void daw_throw_on_null( ValueType const *const value,
		                                  StringType const &msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename Bool, typename... Args>
		constexpr void daw_throw_on_null( ValueType *value, StringType &&format,
		                                  Args... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType>
		constexpr ValueType *daw_throw_on_null_or_return( ValueType *value,
		                                                  StringType const &msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename Bool, typename... Args>
		constexpr ValueType *daw_throw_on_null_or_return( ValueType *value,
		                                                  StringType &&format,
		                                                  Args &&... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename StringType,
		         typename Bool, typename... Args>
		constexpr bool daw_throw_on_false_or_return( Bool &&test,
		                                             StringType &&format,
		                                             Args &&... args ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return true;
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename Bool>
		constexpr ValueType daw_throw_on_false_or_return( ValueType &&value,
		                                                  Bool &&test,
		                                                  StringType msg ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename Bool, typename... Args>
		constexpr ValueType
		daw_throw_on_false_or_return( ValueType &&value, Bool &&test,
		                              StringType &&format, Args &&... args ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr void daw_throw_on_false( Bool const &test,
		                                   StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr void Assert( Bool const &test, StringType const &msg ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType, typename... Args>
		constexpr void daw_throw_on_false( Bool const &test, StringType &&format,
		                                   Args &&... args ) {
			if( !static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
		}

		template<typename ExceptionType = std::exception, typename Bool>
		constexpr void daw_throw_on_false( Bool const &test ) {
			if( !static_cast<bool>( test ) ) {
				throw ExceptionType{};
			}
		}

		template<typename ExceptionType = std::exception, typename Bool>
		constexpr void daw_throw_on_true( Bool const &test ) {
			if( static_cast<bool>( test ) ) {
				throw ExceptionType{};
			}
		}

		template<typename ExceptionType>
		constexpr void daw_throw_value_on_true( ExceptionType const &test ) {
			if( static_cast<bool>( test ) ) {
				throw test;
			}
		}

		template<typename ExceptionType>
		constexpr void daw_throw_value_on_false( ExceptionType const &test ) {
			if( !static_cast<bool>( test ) ) {
				throw test;
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
		         typename StringType, typename Bool, typename... Args>
		constexpr ValueType
		daw_throw_on_true_or_return( ValueType &&value, Bool &&test,
		                             StringType &&format, Args &&... args ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename Bool>
		constexpr ValueType &&daw_throw_on_true_or_return( ValueType &&value,
		                                                   Bool &&test,
		                                                   StringType msg ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType,
		         typename StringType, typename... Args>
		constexpr ValueType
		daw_throw_on_true_or_return( ValueType &&value, bool test,
		                             StringType &&format, Args &&... args ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr bool daw_throw_on_true_or_return( Bool const &test,
		                                            StringType const &msg ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType, typename... Args>
		constexpr bool daw_throw_on_true_or_return( Bool const &test,
		                                            StringType &&format,
		                                            Args &&... args ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType>
		constexpr void daw_throw_on_true( Bool const &test,
		                                  StringType const &msg ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename Bool,
		         typename StringType, typename... Args>
		constexpr void daw_throw_on_true( Bool const &test, StringType &&format,
		                                  Args &&... args ) {
			if( static_cast<bool>( test ) ) {
				daw_throw<ExceptionType>( daw::string::fmt(
				  std::forward<StringType>( format ), std::forward<Args>( args )... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename Predicate,
		         typename StringType, typename Container>
		constexpr void assert_all_false( Container const &container,
		                                 StringType &&assert_message,
		                                 Predicate predicate ) {
			if( std::find_if( std::begin( container ), std::end( container ),
			                  predicate ) != std::end( container ) ) {
				daw_throw<ExceptionType>( std::forward<StringType>( assert_message ) );
			}
		}

		template<typename ExceptionType = AssertException, typename Predicate,
		         typename StringType, typename Container>
		constexpr void assert_all_true( Container const &container,
		                                StringType &&assert_message,
		                                Predicate predicate ) {
			assert_all_false(
			  container, std::forward<StringType>( assert_message ),
			  [predicate]( auto const &v ) { return !predicate( v ); } );
		}

		template<typename Function, typename... Args>
		void no_exception( Function func, Args &&... args ) noexcept {
			try {
				func( std::forward<Args>( args )... );
			} catch( ... ) {}
		}

	} // namespace exception
} // namespace daw

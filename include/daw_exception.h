#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2016 Darrell Wright
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

#include "daw_string.h"
#include <stdexcept>
#include <string>

namespace daw {
	namespace exception {
#define MAKE_DAW_EXCEPTION( EXCEPTION_TYPE )                                                                           \
	namespace impl {                                                                                                   \
		template<typename MsgType>                                                                                     \
		struct EXCEPTION_TYPE final {                                                                                  \
			MsgType message;                                                                                           \
			EXCEPTION_TYPE( std::string msg ) : message{std::move( msg )} {}                                           \
			EXCEPTION_TYPE( ) = default;                                                                               \
			~EXCEPTION_TYPE( ) = default;                                                                              \
			EXCEPTION_TYPE( EXCEPTION_TYPE const & ) = default;                                                        \
			EXCEPTION_TYPE( EXCEPTION_TYPE && ) = default;                                                             \
			EXCEPTION_TYPE &operator=( EXCEPTION_TYPE const & ) = default;                                             \
			EXCEPTION_TYPE &operator=( EXCEPTION_TYPE && ) = default;                                                  \
			std::string const &what( ) {                                                                               \
				return message;                                                                                        \
			}                                                                                                          \
		};                                                                                                             \
	}                                                                                                                  \
	using EXCEPTION_TYPE = ::daw::exception::impl::EXCEPTION_TYPE<std::string>;

		MAKE_DAW_EXCEPTION( NotImplemented );
		MAKE_DAW_EXCEPTION( FatalError );
		MAKE_DAW_EXCEPTION( NullPtrAccessException );
		MAKE_DAW_EXCEPTION( AssertException );
		MAKE_DAW_EXCEPTION( FileException );

#undef MAKE_DAW_EXCEPTION

		template<typename ExceptionType = std::runtime_error, typename StringType>
		void daw_throw( StringType const &msg ) {
			throw ExceptionType( msg );
		}

		template<typename ExceptionType = std::runtime_error, typename StringType>
		void debug_throw( StringType const &msg ) {
#ifndef NODEBUGTHROW
			daw_throw<ExceptionType>( msg );
#else
			return;
#endif
		}

		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		void dbg_throw_on_null( ValueType const *const value, StringType const &msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		ValueType *dbg_throw_on_null_or_return( ValueType *value, StringType const &msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType &dbg_throw_on_false_or_return( ValueType &&value, bool test, StringType const &msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
			return std::forward<ValueType>( value );
		}

		template<typename ExceptionType = AssertException, typename StringType>
		bool dbg_throw_on_false_or_return( bool test, StringType const &msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
			return true;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void dbg_throw_on_false( bool test, StringType const &msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void dbg_throw_on_false( bool test, StringType const &format, Arg arg, Args... args ) {
			if( !test ) {
				debug_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType &dbg_throw_on_true_or_return( ValueType &value, bool test, StringType const &msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename BoolType, typename StringType>
		bool dbg_throw_on_true_or_return( const BoolType &test, StringType const &msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
			return test;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void dbg_throw_on_true( bool test, StringType const &msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void dbg_throw_on_true( bool test, StringType const &format, Arg arg, Args... args ) {
			if( test ) {
				debug_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		void daw_throw_on_null( ValueType const *const value, StringType const &msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType,
		         typename Arg, typename... Args>
		void daw_throw_on_null( ValueType *value, StringType format, Arg arg, Args... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType *daw_throw_on_null_or_return( ValueType *value, StringType const &msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType,
		         typename Arg, typename... Args>
		ValueType *daw_throw_on_null_or_return( ValueType *value, StringType format, Arg arg, Args... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename StringType, typename BoolType, typename Arg,
		         typename... Args>
		bool daw_throw_on_false_or_return( BoolType &&test, StringType format, Arg arg, Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return true;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType>
		ValueType &daw_throw_on_false_or_return( ValueType &&value, BoolType &&test, StringType msg ) {
			if( !test ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType,
		         typename Arg, typename... Args>
		ValueType &daw_throw_on_false_or_return( ValueType &&value, BoolType &&test, StringType format, Arg arg,
		                                         Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename BoolType, typename StringType>
		void daw_throw_on_false( BoolType const &test, StringType const &msg ) {
			if( !test ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void daw_throw_on_false( bool test, StringType const &format, Arg arg, Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException>
		void daw_throw_on_false( bool test ) {
			if( !test ) {
				throw ExceptionType{};
			}
		}

		template<typename ExceptionType = AssertException>
		void daw_throw_on_true( bool test ) {
			if( !true ) {
				throw ExceptionType{};
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType,
		         typename Arg, typename... Args>
		ValueType &daw_throw_on_true_or_return( ValueType &&value, BoolType &&test, StringType format, Arg arg,
		                                        Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType>
		ValueType &daw_throw_on_true_or_return( ValueType &&value, BoolType &&test, StringType msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename Arg,
		         typename... Args>
		ValueType &daw_throw_on_true_or_return( ValueType &&value, bool test, StringType const &format, Arg arg,
		                                        Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename StringType>
		bool daw_throw_on_true_or_return( bool test, StringType const &msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		bool daw_throw_on_true_or_return( bool test, StringType const &format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void daw_throw_on_true( bool test, StringType const &msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void daw_throw_on_true( bool test, StringType const &format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::fmt( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename Predicate, typename StringType, typename Container>
		void assert_all_false( Container const &container, StringType &&assert_message, Predicate predicate ) {
			if( std::find_if( std::begin( container ), std::end( container ), predicate ) != std::end( container ) ) {
				daw_throw<ExceptionType>( std::forward<StringType>( assert_message ) );
			}
		}

		template<typename ExceptionType = AssertException, typename Predicate, typename StringType, typename Container>
		void assert_all_true( Container const &container, StringType &&assert_message, Predicate predicate ) {
			assert_all_false( container, std::forward<StringType>( assert_message ),
			                  [predicate]( auto const &v ) { return !predicate( v ); } );
		}

	} // namespace exception
} // namespace daw

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_is_constant_evaluated.h"

#include <exception>
#include <string_view>

namespace daw::ensure {
#if defined( DAW_ATTRIB_ENABLE_IF )
#if defined( DAW_HAS_CPP26_DELETED_REASON )
	[[noreturn]] DAW_ATTRIB_NOINLINE inline void ensure_error( bool b )
	  DAW_ATTRIB_ENABLE_IF( __builtin_constant_p( b ) and b,
	                        "Ensure check failed" ) =
	    delete( "Ensure check failed" );
#endif
	[[noreturn]] DAW_ATTRIB_NOINLINE inline void ensure_error( bool b )
	  DAW_ATTRIB_ENABLE_IF( __builtin_constant_p( b ) and not b,
	                        "Ensure check failed" ) {
		std::terminate( );
	}

	[[noreturn]] DAW_ATTRIB_NOINLINE inline void ensure_error( bool b )
	  DAW_ATTRIB_ENABLE_IF( not __builtin_constant_p( b ), " " ) {
		std::terminate( );
	}
#else
	[[noreturn]] DAW_ATTRIB_NOINLINE inline void ensure_error( bool ) {
		std::terminate( );
	}
#endif

#if defined( DAW_HAS_GCC_LIKE )
	[[gnu::error( "Ensure check failed at compile time" )]]
	extern void ensure_compile_error( bool );
#endif

	struct daw_ensure_exception {};
} // namespace daw::ensure

#if defined( DAW_HAS_IS_CONSTANT_EVALUATED )
#define DAW_ENSURE_IS_CONSTANT_EVAL( ) DAW_IS_CONSTANT_EVALUATED( )
#else
#define DAW_ENSURE_IS_CONSTANT_EVAL( ) false
#endif

#if not defined( DAW_NO_OPT_TIME_ENSURE ) and defined( DAW_HAS_GCC_LIKE )
#define daw_ensure( ... )                                               \
	do {                                                                  \
		if( DAW_ENSURE_IS_CONSTANT_EVAL( ) ) {                              \
			if( not( __VA_ARGS__ ) ) {                                        \
				throw ::daw::ensure::daw_ensure_exception{ };                   \
			}                                                                 \
		} else {                                                            \
			if( auto daw_ensure_bool_test = static_cast<bool>( __VA_ARGS__ ); \
			    __builtin_constant_p( daw_ensure_bool_test ) ) {              \
				if( not( daw_ensure_bool_test ) ) {                             \
					::daw::ensure::ensure_compile_error( not( __VA_ARGS__ ) );    \
				}                                                               \
			} else {                                                          \
				if( not( daw_ensure_bool_test ) ) {                             \
					::daw::ensure::ensure_error( not( __VA_ARGS__ ) );            \
				}                                                               \
			}                                                                 \
		}                                                                   \
	} while( false )

#if not defined( NDEBUG )
#define daw_dbg_ensure( ... ) daw_ensure( __VA_ARGS__ )
#else
#define daw_dbg_ensure( ... )                                        \
	do {                                                               \
		if( DAW_ENSURE_IS_CONSTANT_EVAL( ) ) {                           \
			if( not( __VA_ARGS__ ) ) {                                     \
				throw ::daw::ensure::daw_ensure_exception{ };                \
			}                                                              \
		} else {                                                         \
			if( __builtin_constant_p( __VA_ARGS__ ) ) {                    \
				if( not( __VA_ARGS__ ) ) {                                   \
					::daw::ensure::ensure_compile_error( not( __VA_ARGS__ ) ); \
				}                                                            \
			}                                                              \
		}                                                                \
	} while( false )
#endif
#else
#define daw_ensure( ... )                                  \
	do {                                                     \
		if( DAW_ENSURE_IS_CONSTANT_EVAL( ) ) {                 \
			if( not( __VA_ARGS__ ) ) {                           \
				throw ::daw::ensure::daw_ensure_exception{ };      \
			}                                                    \
		} else {                                               \
			if( not( __VA_ARGS__ ) ) {                           \
				::daw::ensure::ensure_error( not( __VA_ARGS__ ) ); \
			}                                                    \
		}                                                      \
	} while( false )

#if not defined( NDEBUG )
#define daw_dbg_ensure( ... )                              \
	do {                                                     \
		if( DAW_ENSURE_IS_CONSTANT_EVAL( ) ) {                 \
			if( not( __VA_ARGS__ ) ) {                           \
				throw ::daw::ensure::daw_ensure_exception{ };      \
			}                                                    \
		} else {                                               \
			if( not( __VA_ARGS__ ) ) {                           \
				::daw::ensure::ensure_error( not( __VA_ARGS__ ) ); \
			}                                                    \
		}                                                      \
	} while( false )
#else
#define daw_dbg_ensure( ... )                         \
	do {                                                \
		if( DAW_ENSURE_IS_CONSTANT_EVAL( ) ) {            \
			if( not( __VA_ARGS__ ) ) {                      \
				throw ::daw::ensure::daw_ensure_exception{ }; \
			}                                               \
		}                                                 \
	} while( false )
#endif
#endif
